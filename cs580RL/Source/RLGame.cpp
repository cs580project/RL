#include "DXUT.h"
#include "RLGame.h"
#include "database.h"
#include "terrain.h"

extern unsigned int			g_catWin;
extern unsigned int			g_mouseWin;

static const int cSpeedSingleStep = 1;
static const int cSpeedSlow       = 10;
static const int cSpeedMedium     = 100;
static const int cSpeedFast       = 1000;
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


void RLGame::init()
{
	m_learningWorld = RLWorld();
	m_RLearner      = RLearner(m_learningWorld);
}

bool RLGame::States(State_Machine_Event event, MSG_Object * msg, int state, int substate)
{
    BeginStateMachine

	//Global message response section
	OnMsg( MSG_Reset )
		ResetStateMachine();
        ChangeState(STATE_Initialize);


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

        OnMsg(MSG_SetPunish)
            m_punishmentValue = msg->GetFloatData();

        OnMsg(MSG_SetReward)
            m_rewardValue = msg->GetFloatData();

        OnMsg(MSG_SetTrainLoop)
            m_trainingIterations = msg->GetIntData();

        OnMsg(MSG_SetMethod_UseQL)
            if (msg->GetBoolData()) m_learningMethod = LearningMethod::Q_LEARNING;
            else m_learningMethod = LearningMethod::SARSA;


        // TODO: Integrate the branches
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


        OnMsg(MSG_StartLearning)
            ChangeState(STATE_Learning);

        OnMsg(MSG_StartPlaying)
            ChangeState(STATE_Playing);


	///////////////////////////////////////////////////////////////
    DeclareState( STATE_Learning )

        DeclareStateInt(iterations)

        OnEnter
            m_RLearner.SetRunning(true);
            iterations = 0;
			m_RLearner.getWorld().resetGame();
			m_RLearner.RunTraining(200000);
			m_RLearner.SetRunning(false);
			ChangeState(STATE_Waiting);
			g_catWin = m_RLearner.getWorld().catScores;
			g_mouseWin = m_RLearner.getWorld().mouseScores;

        //OnUpdate
        //    int potentialIterations = iterations + m_iterationsPerFrame;
        //    if (potentialIterations >= m_trainingIterations)
        //    {
        //        m_RLearner.RunTraining(potentialIterations - m_trainingIterations);

        //        // TODO: Signal completion of learning algorithm

        //        m_RLearner.SetRunning(false);
        //        ChangeState(STATE_Waiting);
        //    }
        //    else
        //    {
        //        m_RLearner.RunTraining(m_iterationsPerFrame);
        //        iterations += m_iterationsPerFrame;

        //        // TODO: Signal "teleport"
        //    }



	///////////////////////////////////////////////////////////////
		DeclareState(STATE_Playing)

		OnEnter
				//m_RLearner.SetRunning(false);
				//m_RLearner.SetPlaying(true);
			m_learningWorld.resetState();
            // TODO: Reset to starting positions

		OnPeriodicTimeInState(0.5)
		if (!m_learningWorld.endState())
		{
			int   action = m_RLearner.SelectAction(m_learningWorld.getCurrentState());
			m_learningWorld.getNextState(action);
			m_learningWorld.drawState();
		}
		else
		{
			ChangeState(STATE_Waiting);
		}
            // TODO: Update each frame based on policy


    EndStateMachine
}


