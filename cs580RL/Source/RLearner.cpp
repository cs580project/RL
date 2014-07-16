#include "DXUT.h"
#include "RLearner.h"
#include <time.h> 

RLearner::RLearner() :
    m_learningWorld(),
    m_policy(8),
    m_running(false),
    m_playing(false),
    m_alpha(1.0f), // TODO: Make these adjustable, add Epsilon, Lambda.
    m_gamma(0.1f),
	m_epsilon(0.1f),
	m_learningMethod(Q_LEARNING),
	selectActionMethod(E_GREEDY)
{

}


RLearner::~RLearner()
{

}

void RLearner::RunEpoch(LearningMethod method)
{
	m_learningWorld.ResetState();

	switch (method)
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
    
	float thisQ = 0.0f;
	float maxQ  = 0.0f;
	float newQ  = 0.0f;

	while (!m_learningWorld.EndState())
	{
        if (!m_running)
        {
            return;
        }

		vector<int>     state       = m_learningWorld.GetCurrentState();
		int             action      = SelectAction(state);
		vector<int>&    newstate = m_learningWorld.GetNextState(action, true);
		float           reward      = m_learningWorld.GetReward();

		thisQ   = m_policy.getQValue(state, action);
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
	if (m_playing)
	{
		return;
	}

	int newAction;
	float thisQ = 0.f;
	float nextQ = 0.f;
	float newQ = 0.f;

	vector<int>     state = m_learningWorld.GetCurrentState();
	int   action = SelectAction(state);
	while (!m_learningWorld.EndState())
	{
		if (!m_running)
		{
			return;
		}

		vector<int>&    newstate = m_learningWorld.GetNextState(action, true);
		float           reward = m_learningWorld.GetReward();

		newAction = SelectAction(newstate);

		thisQ = m_policy.getQValue(state, action);
		nextQ = m_policy.getQValue(newstate, newAction);
		
	}
}

int RLearner::SelectAction(vector<int>& state)
{
	int selectedAction = -1;

	switch (selectActionMethod)
	{
	case E_GREEDY:

		//if (rand() % 1000 > 0)
		{
			vector<float>& qvalues = m_policy.getQValues(state);

			float   maxQ = -100000;

			for (int i = 0; i < qvalues.size(); ++i)
			{
				if (qvalues[i]>maxQ&&m_learningWorld.ValidAction(i))
				{
					maxQ = qvalues[i];
					selectedAction = i;
				}
				else if (qvalues[i] == maxQ&&m_learningWorld.ValidAction(i))
				{
					if (rand() % 2 == 0)
						selectedAction = i;
				}
			}
		}
		break;
	
	case SOFTMAX:
		break;
	}

	if (selectedAction == -1)
	{
		do
		{
			selectedAction = rand() % m_policy.getActionNum();
		} while (!m_learningWorld.ValidAction(selectedAction));
	}

	return selectedAction;
}

void RLearner::RunTraining(int numberOfEpochs, LearningMethod method)
{

    if (m_playing)
    {
        return;
    }

	for (int i = 0; i < numberOfEpochs; ++i)
    {
        if (!m_running)
        {
            return;
        }

		RunEpoch(method);
	}
}

void RLearner::reset()
{
	m_running = false;
	m_policy.resetToDefault();
	m_learningWorld.ResetAll();
}

