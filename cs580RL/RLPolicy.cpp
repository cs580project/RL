#include "DXUT.h"
#include "RLPolicy.h"


RLPolicy::RLPolicy(int _actionNum) :actionNum(_actionNum)
{
}


RLPolicy::~RLPolicy()
{
}

vector<float>& RLPolicy::getQValue(vector<int> state)
{
	return qValueTable[state];
}

void RLPolicy::setQValue(vector<int> state, int acton, float qvalue)
{
	vector<float>& qvalues = qValueTable[state];
	qvalues[acton] = qvalue;
}

int RLPolicy::getBestAction(vector<int> state)
{
	vector<float>& qvalues = qValueTable[state];
	float maxQ = 0;
	int bestAction=0;
	for (int i = 0; i < actionNum; ++i)
	{
		if (qvalues[i]>maxQ)
		{
			maxQ = qvalues[i];
			bestAction = i;
		}
	}
	return bestAction;
}

