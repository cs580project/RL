#include "DXUT.h"
#include "RLPolicy.h"


RLPolicy::RLPolicy(int _actionNum) :actionNum(_actionNum)
{
}


RLPolicy::~RLPolicy()
{
}

vector<float>& RLPolicy::getQValues(vector<int> state)
{
	return qValueTable[state];
}

void RLPolicy::setQValue(vector<int> state, int action, float qvalue)
{
	if (qValueTable.count(state) == 0 && abs(qvalue) <0.00001f)
		return;
	vector<float>& qvalues = qValueTable[state];
	if (qvalues.size() == 0)
	{
		for (int i = 0; i < actionNum; ++i)
			qvalues.push_back(0);
	}
	qvalues[action] = qvalue;
}

int RLPolicy::getBestAction(vector<int> state)
{
	if (qValueTable.count(state) == 0)
		return rand() % actionNum;
	vector<float>& qvalues = qValueTable[state];

	float maxQ = -100000;
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

float RLPolicy::getQValue(vector<int> state, int action)
{
	if (qValueTable.count(state) == 0)
		return 0;
	vector<float>& qvalues = qValueTable[state];
	return qvalues[action];
}

float RLPolicy::getMaxQValue(vector<int> state)
{
	if (qValueTable.count(state) == 0)
		return 0;
	vector<float>& qvalues = qValueTable[state];
	float maxQ = -1;
	int bestAction = 0;
	for (int i = 0; i < actionNum; ++i)
	{
		if (qvalues[i]>maxQ)
		{
			maxQ = qvalues[i];
		}
	}
	return maxQ;

}

