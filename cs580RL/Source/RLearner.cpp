#include "DXUT.h"
#include "RLearner.h"


RLearner::RLearner(RLWorld& world) :
    m_learningWorld(world),
    m_policy(8),
    m_running(false),
    m_playing(false),
    m_alpha(1.0f), // TODO: Make these adjustable, add Epsilon, Lambda.
    m_gamma(0.1f),
	m_learningMethod(Q_LEARNING),
	selectActionMethod(E_GREEDY)

{

}


RLearner::~RLearner()
{

}

void RLearner::RunEpoch()
{
	m_learningWorld.resetState();

	switch (m_learningMethod)
	{
	case Q_LEARNING:
		QLearning();
		break;

	case SARSA:
		Sarsa();
		break;

	case Q_LAMBDA:
		QLambda();
		break;
	}
}

void RLearner::QLearning()
{
    if (m_playing)
    {
        return;
    }
    
	float thisQ = 0.f;
	float maxQ  = 0.f;
	float newQ  = 0.f;

	while (!m_learningWorld.endState())
	{
        if (!m_running)
        {
            return;
        }

		vector<int>&    state       = m_learningWorld.getCurrentState();
		int             action      = SelectAction(state);
		vector<int>&    newstate    = m_learningWorld.getNextState(action);
		float           reward      = m_learningWorld.getReward();

		thisQ   = m_policy.getQValue(state, action);
		if (reward != 0)
			reward = reward;
		maxQ    = m_policy.getMaxQValue(newstate);
		
        newQ    = thisQ + m_alpha*(reward + m_gamma*maxQ - thisQ);
		m_policy.setQValue(state, action, newQ);
	}
}

void RLearner::QLambda()
{

}

void RLearner::Sarsa()
{

}

int RLearner::SelectAction(vector<int>& state)
{
	int selectedAction = 0;

	switch (selectActionMethod)
	{
	case E_GREEDY:
		selectedAction = m_policy.getBestAction(state);
		break;
	
    case SOFTMAX:
		break;
	
    default:
		break;
	}

	return selectedAction;
}

void RLearner::RunTraining(int epochNum)
{
    if (m_playing)
    {
        return;
    }

	for (int i = 0; i < epochNum; ++i)
    {
        if (!m_running)
        {
            return;
        }

		RunEpoch();
	}
}

