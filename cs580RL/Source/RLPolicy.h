#pragma once

#include<map>
#include<vector>

using namespace std;

class RLPolicy
{
public:

	RLPolicy(){}
	RLPolicy(int actionNum);
	~RLPolicy();

	vector<float>&  getQValues(vector<int> state);
	float           getQValue(vector<int> state, int action);
	void            setQValue(vector<int> state, int action, float qvalue);
	int				getBestAction(vector<int> state);
	float           getMaxQValue(vector<int> state);
    void            resetToDefault();

    int				getActionNum()  { return actionNum; }

private:

	map<vector<int>, vector<float>> qValueTable;

	vector<float> emptyState;
	int actionNum;
};

