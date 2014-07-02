#include "DXUT.h"
#include "RLearner.h"


RLearner::RLearner() :policy(6)
{
}


RLearner::~RLearner()
{
}

void RLearner::runEpoch()
{
	theWorld.resetState();
	switch (learningMethod)
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
	float thisQ;
	float maxQ;
	float newQ;
	while (!theWorld.endState())
	{
		if (!running) return;
		vector<int>& state = theWorld.getCurrentState();
		int action = selectAction(state);
		vector<int>& newstate = theWorld.getNextState(action);
		float reward = theWorld.getReward();

		thisQ = policy.getQValue(state, action);
		maxQ = policy.getMaxQValue(newstate);

		newQ = thisQ + alpha*(reward + gamma*maxQ - thisQ);
		policy.setQValue(state, action, newQ);
	}
}

void RLearner::QLambda()
{

}

void RLearner::Sarsa()
{

}

int RLearner::selectAction(vector<int>& state)
{
	int selectedAction = 0;
	switch (selectActionMethod)
	{
	case E_GREEDY:
		selectedAction = policy.getBestAction(state);
		break;
	case SOFTMAX:
		break;
	default:
		break;
	}
	return selectedAction;
}

