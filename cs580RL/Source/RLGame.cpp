#include "DXUT.h"
#include "RLGame.h"
#include "database.h"
#include "terrain.h"

extern float	    g_reward;
extern float        g_punish;
extern unsigned int	g_catWin;
extern unsigned int	g_mouseWin;
extern unsigned int	g_trainCatWin;
extern unsigned int	g_trainMouseWin;
extern unsigned int g_trainingStatus;

static const int cSpeedSuperSlow  = -16;
static const int cSpeedSlow       = -4;
static const int cSpeedMedium     = -1;
static const int cSpeedFast       =  1;
static const int cSpeedTurbo      =  10000;

//Add new states below
enum StateName {
    STATE_Initialize,	//The first enum is the starting state
    STATE_Waiting,
    STATE_Learning,
    STATE_Playing,
    STATE_EndGame
};


//Add new substates below
enum SubstateName {		//Order doesn't matter
    //empty
};



RLGame::RLGame(GameObject & object) :
    StateMachine(object),
    m_RLearner(),
    m_punishmentValue(0.f),
    m_rewardValue(0.f),
    m_playIsContinuous(true),
    m_useSmartMouse(false)
{

}

bool RLGame::States(State_Machine_Event event, MSG_Object * msg, int state, int substate)
{
    BeginStateMachine

	//Global message response section
	OnMsg( MSG_Reset )
		ResetStateMachine();
        m_RLearner.GetPolicy().resetToDefault();
        ChangeState(STATE_Initialize);

	OnMsg(MSG_ResetLearner)
		m_RLearner.reset();
		g_catWin = 0;
		g_mouseWin = 0;
		g_trainCatWin = 0;
		g_trainMouseWin = 0;

    OnMsg(MSG_ClearScores)
        g_catWin = 0;
        g_mouseWin = 0;
        g_trainCatWin = 0;
        g_trainMouseWin = 0;
        m_RLearner.getWorld().ResetScores();

    OnMsg(MSG_SetRLSpeed)
        switch (static_cast<RLSpeed>(msg->GetIntData()))
        {
            case RLSpeed::Slow:
                m_iterationsPerFrame = cSpeedSlow;
                break;

            case RLSpeed::Medium:
                m_iterationsPerFrame = cSpeedMedium;
                break;

            case RLSpeed::Fast:
                m_iterationsPerFrame = cSpeedFast;
                break;

            case RLSpeed::Turbo:
                m_iterationsPerFrame = cSpeedTurbo;
                break;

            case RLSpeed::SuperSlow:
            default:
                m_iterationsPerFrame = cSpeedSuperSlow;
                break;
        }

    OnMsg(MSG_SetPlayContinuous)
        m_playIsContinuous = msg->GetBoolData();


    OnMsg(MSG_UseSmartMouse)
        m_useSmartMouse = msg->GetBoolData();


    OnMsg(MSG_StartLearning)
        if (m_RLearner.GetRunning())
        {
            m_RLearner.SetRunning(false);
            g_trainingStatus = 0;
            ChangeState(STATE_Waiting);
        }
        else
        {
            ChangeState(STATE_Learning);
        }

    OnMsg(MSG_StartPlaying)
        if (m_RLearner.GetPlaying())
        {
            m_RLearner.SetPlaying(false);
            g_trainingStatus = 0;
            ChangeState(STATE_Waiting);
        }
        else
        {
            ChangeState(STATE_Playing);
        }

	///////////////////////////////////////////////////////////////
    DeclareState(STATE_Initialize)

        OnEnter
            m_punishmentValue = g_punish;
            m_rewardValue = g_reward;
            m_trainingIterations = 1000;
            m_learningMethod = LearningMethod::Q_LEARNING;
            m_iterationsPerFrame = cSpeedMedium;
            m_RLearner.SetRunning(false);
            g_trainingStatus = 0;
            ChangeState(STATE_Waiting);


    ///////////////////////////////////////////////////////////////
    DeclareState(STATE_Waiting)

        OnEnter
            m_RLearner.SetRunning(false);
            m_RLearner.SetPlaying(false);

        OnMsg(MSG_SetPunish)
            m_punishmentValue = msg->GetFloatData();

        OnMsg(MSG_SetReward)
            m_rewardValue = msg->GetFloatData();

        OnMsg(MSG_SetTrainLoop)
            m_trainingIterations = msg->GetIntData();

        OnMsg(MSG_SetMethod_UseQL)
            if (msg->GetBoolData()) m_learningMethod = LearningMethod::Q_LEARNING;
            else m_learningMethod = LearningMethod::SARSA;


	///////////////////////////////////////////////////////////////
    DeclareState(STATE_Learning)

        DeclareStateInt(iterations)
        DeclareStateInt(intermediateIterations)

        OnEnter
            if (m_RLearner.GetPlaying())
            {
                PopState();
            }

            m_RLearner.SetRunning(true);
            iterations = 0;
            intermediateIterations = 0;
            g_trainingStatus = 1;

        OnUpdate

            // This adds intermediate frames in the case we want each one to be visible. 
            // The intermediate period is ignored when we're not in a "super slow" mode.
            if (m_iterationsPerFrame <= 0)
            {
                int frameDelay = -1 * m_iterationsPerFrame;

                if (intermediateIterations >= frameDelay)
                {
                    intermediateIterations = 0;

                    m_RLearner.getWorld().SetRewardVal(m_rewardValue);
                    m_RLearner.getWorld().SetPunishVal(m_punishmentValue);
                    m_RLearner.RunTraining(1, m_learningMethod);
                    ++iterations;

                    g_trainCatWin   = m_RLearner.getWorld().GetCatTrainingScore();
                    g_trainMouseWin = m_RLearner.getWorld().GetMouseTrainingScore();

                    // Signal "teleport"
                    m_RLearner.getWorld().DrawRLState(true);
                }
                else
                {
                    ++intermediateIterations;
                }
            }
            else
            {
                int potentialIterations = iterations + m_iterationsPerFrame;

                if (potentialIterations >= m_trainingIterations)
                {
                    m_RLearner.getWorld().SetRewardVal(m_rewardValue);
                    m_RLearner.getWorld().SetPunishVal(m_punishmentValue);
                    m_RLearner.RunTraining(m_trainingIterations - iterations, m_learningMethod);

                    // Signal "teleport"
                    m_RLearner.getWorld().DrawRLState(true);

                    m_RLearner.SetRunning(false);
                    g_trainingStatus = 2;
                    ChangeState(STATE_Waiting);
                }
                else
                {
                    m_RLearner.getWorld().SetRewardVal(m_rewardValue);
                    m_RLearner.getWorld().SetPunishVal(m_punishmentValue);
                    m_RLearner.RunTraining(m_iterationsPerFrame, m_learningMethod);

                    iterations += m_iterationsPerFrame;

                    // Signal "teleport"
                    m_RLearner.getWorld().DrawRLState(true);
                }

                g_trainCatWin = m_RLearner.getWorld().GetCatTrainingScore();
                g_trainMouseWin = m_RLearner.getWorld().GetMouseTrainingScore();
            }


	///////////////////////////////////////////////////////////////
	DeclareState(STATE_Playing)

        DeclareStateBool(startPos)
        DeclareStateFloat(elapsedTime)
        DeclareStateInt(lastIterationsPerFrame)
        DeclareStateBool(loop)
        DeclareStateInt(gamesLeftToPlay)
        DeclareStateInt(gamesLeftToPlayPrev)
        DeclareStateBool(wasPlayingContinuouslyLastFrame)

		OnEnter
            if (m_RLearner.GetRunning())
            {
                PopState();
            }

            lastIterationsPerFrame = -INT_MAX;
            startPos = true;
            elapsedTime = 0.f;
			m_RLearner.SetPlaying(true);
            m_RLearner.getWorld().ResetState(); // Resets starting positions.

            g_trainingStatus = 3;

            // force it to update this state on the first frame
            wasPlayingContinuouslyLastFrame = !m_playIsContinuous;
            gamesLeftToPlayPrev = m_trainingIterations;

		OnUpdate

            if (m_iterationsPerFrame != lastIterationsPerFrame)
            {
                lastIterationsPerFrame = m_iterationsPerFrame;

                if (m_iterationsPerFrame < cSpeedMedium)
                {
                    g_database.SendMsgFromSystem(10, MSG_SetAgentSpeed, MSG_Data(false));
                    g_database.SendMsgFromSystem(11, MSG_SetAgentSpeed, MSG_Data(false));
                }
                else if (m_iterationsPerFrame < cSpeedFast)
                {
                    g_database.SendMsgFromSystem(10, MSG_SetAgentSpeed, MSG_Data(true));
                    g_database.SendMsgFromSystem(11, MSG_SetAgentSpeed, MSG_Data(true));
                }
                else if (m_iterationsPerFrame < cSpeedTurbo)
                {
                    g_database.SendMsgFromSystem(10, MSG_Reset, MSG_Data(true));
                    g_database.SendMsgFromSystem(11, MSG_Reset, MSG_Data(true));
                }
                else
                {
                    loop = true;
                }
            }

            if (wasPlayingContinuouslyLastFrame != m_playIsContinuous)
            {
                if (m_playIsContinuous)
                {
                    // pick up where it left off for iterations left
                    gamesLeftToPlay = gamesLeftToPlayPrev;
                }
                else
                {
                    // just finish the current game
                    gamesLeftToPlayPrev = gamesLeftToPlay;
                    gamesLeftToPlay = 1;
                }
            }

            if (loop)
            {
                while (gamesLeftToPlay > 0)
                {
                    static const int maxRoundsBeforeTie = 1000;
                    for (int j = 0; j < maxRoundsBeforeTie; ++j)
                    {
                        int action;
                        if (m_useSmartMouse)
                        {
                            action = m_RLearner.SelectAction(m_RLearner.getWorld().GetCurrentState());
                        }
                        else
                        {
                            action = m_RLearner.getWorld().SelectGreedyAction();
                        }

                        m_RLearner.getWorld().GetNextState(action, false);

                        if (m_RLearner.getWorld().EndState())
                        {
                            break;
                        }
                    }

                    g_catWin = m_RLearner.getWorld().GetCatPlayingScore();
                    g_mouseWin = m_RLearner.getWorld().GetMousePlayingScore();
                    m_RLearner.getWorld().ResetState(); // Resets starting positions.
                    --gamesLeftToPlay;
                }

                g_trainingStatus = 0;
                ChangeState(STATE_Waiting);
            }
            else
            {
                static const float cWalkSpeed       = 0.40f;
                static const float cJogSpeed        = 0.25f;
                static const float cTeleportSpeed   = 0.00f;

                float waitTime;
                if (m_iterationsPerFrame < cSpeedMedium)
                {
                    waitTime = cWalkSpeed;
                }
                else if (m_iterationsPerFrame < cSpeedFast)
                {
                    waitTime = cJogSpeed;
                }
                else if (m_iterationsPerFrame < cSpeedTurbo)
                {
                    waitTime = cTeleportSpeed;
                }
                else
                {
                    // Iterations per frame - see above
                }

                elapsedTime += g_time.GetElapsedTime();

                if (elapsedTime > waitTime)
                {
                    elapsedTime -= waitTime;

                    if (m_RLearner.getWorld().EndState())
                    {
                        g_catWin = m_RLearner.getWorld().GetCatPlayingScore();
                        g_mouseWin = m_RLearner.getWorld().GetMousePlayingScore();
                        m_RLearner.getWorld().ResetState(); // Resets starting positions.
                        startPos = true;
                        --gamesLeftToPlay;
                    }
                    else
                    {
                        int action;
                        if (m_useSmartMouse)
                        {
                            action = m_RLearner.SelectAction(m_RLearner.getWorld().GetCurrentState());
                        }
                        else
                        {
                            action = m_RLearner.getWorld().SelectGreedyAction();
                        }

                        m_RLearner.getWorld().GetNextState(action, false);

                        if (m_iterationsPerFrame < cSpeedFast)
                        {
                            // Add waypoints
                            m_RLearner.getWorld().DrawRLState(startPos);
                        }
                        else if (m_iterationsPerFrame < cSpeedTurbo)
                        {
                            // Teleport instantly
                            m_RLearner.getWorld().DrawRLState(true);
                        }

                        if (startPos)
                        {
                            startPos = false;
                        }

                        // Always update the mouse's score, since it can change before game ends.
                        g_mouseWin = m_RLearner.getWorld().GetMousePlayingScore();
                    }
                }

                if (gamesLeftToPlay <= 0)
                {
                    g_trainingStatus = 0;
                    ChangeState(STATE_Waiting);
                }
            }

    EndStateMachine
}


void RLGame::ChangeAgentSpeeds()
{
    if (m_iterationsPerFrame < cSpeedMedium)
    {
        g_database.SendMsgFromSystem(10, MSG_SetAgentSpeed, MSG_Data(false));
        g_database.SendMsgFromSystem(11, MSG_SetAgentSpeed, MSG_Data(false));
    }
    else if (m_iterationsPerFrame < cSpeedFast)
    {
        g_database.SendMsgFromSystem(10, MSG_SetAgentSpeed, MSG_Data(true));
        g_database.SendMsgFromSystem(11, MSG_SetAgentSpeed, MSG_Data(true));
    }
    else
    {
        // doesn't matter, will run all iterations before allowing a frame update.
    }
}