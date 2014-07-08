#include "DXUT.h"
#include "RLGame.h"
#include "database.h"
#include "terrain.h"

extern unsigned int			g_catWin;
extern unsigned int			g_mouseWin;

static const int cSpeedSingleStep = 1;
static const int cSpeedSlow       = 2;
static const int cSpeedMedium     = 16;
static const int cSpeedFast       = 32;
static const int cSpeedTurbo      = 128;

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

            case RLSpeed::SingleStep:
            default:
                m_iterationsPerFrame = cSpeedSingleStep;
                break;
        }

	///////////////////////////////////////////////////////////////
    DeclareState(STATE_Initialize)

        OnEnter
            m_punishmentValue = 0.0f;
            m_rewardValue = 0.0f;
            m_trainingIterations = 2000;
            m_learningMethod = LearningMethod::Q_LEARNING;
            m_iterationsPerFrame = 1;
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

        OnEnter
            if (m_RLearner.GetPlaying())
            {
                PopState();
            }

            m_RLearner.SetRunning(true);
            m_RLearner.getWorld().ResetGame();
            iterations = 0;

        OnUpdate
            int potentialIterations = iterations + m_iterationsPerFrame;
            
            if (potentialIterations >= m_trainingIterations)
            {
                m_RLearner.RunTraining(potentialIterations - m_trainingIterations);

                // TODO: Signal completion of learning algorithm
                g_catWin    = m_RLearner.getWorld().catScores;
                g_mouseWin  = m_RLearner.getWorld().mouseScores;

                m_RLearner.SetRunning(false);
                ChangeState(STATE_Waiting);
            }
            else
            {
                m_RLearner.RunTraining(m_iterationsPerFrame);

                iterations += m_iterationsPerFrame;

                g_catWin    = m_RLearner.getWorld().catScores;
                g_mouseWin  = m_RLearner.getWorld().mouseScores;

                // Signal "teleport"
                m_learningWorld.DrawRLState(true);
            }



	///////////////////////////////////////////////////////////////
		DeclareState(STATE_Playing)

            DeclareStateBool(StartPos)

		OnEnter
            if (m_RLearner.GetRunning())
            {
                PopState();
            }

            StartPos = true;
			m_RLearner.SetPlaying(true);
			m_learningWorld.ResetState(); // Resets starting positions.

		OnPeriodicTimeInState(0.4)
		    if (m_learningWorld.EndState())
		    {
			    ChangeState(STATE_Waiting);
		    }
            else
            {
                int action = m_RLearner.SelectAction(m_learningWorld.GetCurrentState());

                m_learningWorld.GetNextState(action);

                // Add waypoints
                m_learningWorld.DrawRLState(StartPos);
                
                if (StartPos)
                {
                    StartPos = false;
                }
            }


    EndStateMachine
}


