#include "DXUT.h"
#include "RLGame.h"
#include "database.h"
#include "terrain.h"

extern unsigned int	g_catWin;
extern unsigned int	g_mouseWin;

static const int cSpeedSuperSlow  = -16;
static const int cSpeedSlow       = -4;
static const int cSpeedMedium     = -1;
static const int cSpeedFast       = 1;
static const int cSpeedTurbo      = 10000;

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
    m_learningWorld(),
    m_RLearner(m_learningWorld)
{

}

bool RLGame::States(State_Machine_Event event, MSG_Object * msg, int state, int substate)
{
    BeginStateMachine

	//Global message response section
	OnMsg( MSG_Reset )
		ResetStateMachine();
        ChangeState(STATE_Initialize);

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

	///////////////////////////////////////////////////////////////
    DeclareState(STATE_Initialize)

        OnEnter
            m_punishmentValue = 0.0f;
            m_rewardValue = 0.0f;
            m_trainingIterations = 1000;
            m_learningMethod = LearningMethod::Q_LEARNING;
            m_iterationsPerFrame = cSpeedMedium;
            m_RLearner.SetRunning(false);
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

        OnMsg(MSG_StartLearning)
            ChangeState(STATE_Learning);

        OnMsg(MSG_StartPlaying)
            ChangeState(STATE_Playing);


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
            m_RLearner.getWorld().ResetGame();
            iterations = 0;
            intermediateIterations = 0;

        OnUpdate

            // This adds intermediate frames in the case we want each one to be visible. 
            // The intermediate period is ignored when we're not in a "super slow" mode.
            if (m_iterationsPerFrame <= 0)
            {
                int frameDelay = -1 * m_iterationsPerFrame;

                if (intermediateIterations >= frameDelay)
                {
                    intermediateIterations = 0;

                    m_RLearner.RunTraining(1, m_learningMethod);
                    ++iterations;

                    g_catWin   = m_RLearner.getWorld().returnCatScore();
                    g_mouseWin = m_RLearner.getWorld().returnMouseScore();

                    // Signal "teleport"
                    m_learningWorld.DrawRLState(true);
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
                    m_RLearner.RunTraining(m_trainingIterations - iterations, m_learningMethod);

                    // TODO: Signal completion of learning algorithm
					g_catWin = m_RLearner.getWorld().returnCatScore();
					g_mouseWin = m_RLearner.getWorld().returnMouseScore();

                    // Signal "teleport"
                    m_learningWorld.DrawRLState(true);

                    m_RLearner.SetRunning(false);
                    ChangeState(STATE_Waiting);
                }
                else
                {
                    m_RLearner.RunTraining(m_iterationsPerFrame, m_learningMethod);

                    iterations += m_iterationsPerFrame;

					g_catWin = m_RLearner.getWorld().returnCatScore();
					g_mouseWin = m_RLearner.getWorld().returnMouseScore();

                    // Signal "teleport"
                    m_learningWorld.DrawRLState(true);
                }
            }


	///////////////////////////////////////////////////////////////
	DeclareState(STATE_Playing)

        DeclareStateBool(startPos)
        DeclareStateFloat(elapsedTime)
        DeclareStateInt(lastIterationsPerFrame)
        DeclareStateBool(loop)

		OnEnter
            if (m_RLearner.GetRunning())
            {
                PopState();
            }

            lastIterationsPerFrame = -INT_MAX;
            startPos = true;
            elapsedTime = 0.f;
			m_RLearner.SetPlaying(true);
			m_learningWorld.ResetState(); // Resets starting positions.

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
                    // teleport
                }
                else
                {
                    loop = true;
                }
            }

            if (loop)
            {
                static const int maxRoundsBeforeTie = 1000;

                for (int i = 0; i < m_iterationsPerFrame; ++i)
                {
                    // this many games  ^^^^^^^^^^^^

                    for (int j = 0; j < maxRoundsBeforeTie; ++j)
                    {
                        int action = m_RLearner.SelectAction(m_learningWorld.GetCurrentState());
                        m_learningWorld.GetNextState(action);

                        if (m_learningWorld.EndState())
                        {
                            // TODO: update score separately from training
                            g_catWin   = m_RLearner.getWorld().returnCatScore();
                            g_mouseWin = m_RLearner.getWorld().returnMouseScore();
                            break;
                        }
                    }
                }

                ChangeState(STATE_Waiting);
            }
            else
            {
                static const float cWalkSpeed = 0.4f;
                static const float cJogSpeed = 0.15f;
                static const float cTeleportSpeed = 0.0f;

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

                    if (m_learningWorld.EndState())
                    {
                        ChangeState(STATE_Waiting);
                    }
                    else
                    {
                        int action = m_RLearner.SelectAction(m_learningWorld.GetCurrentState());

                        m_learningWorld.GetNextState(action);

                        if (m_iterationsPerFrame < cSpeedFast)
                        {
                            // Add waypoints
                            m_learningWorld.DrawRLState(startPos);
                        }
                        else if (m_iterationsPerFrame < cSpeedTurbo)
                        {
                            // Teleport instantly
                            m_learningWorld.DrawRLState(true);
                        }
                        else
                        {
                            // TODO: iterations per frame
                        }

                        if (startPos)
                        {
                            startPos = false;
                        }
                    }
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
        // TODO: Iterations per frame
    }
}