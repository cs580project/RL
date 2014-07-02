#pragma once
#include<map>
#include<vector>
using namespace std;
class RLPolicy
{
public:
	RLPolicy(int actionNum);
	~RLPolicy();

	map<vector<int>, vector<float>> qValueTable;
	vector<float>& getQValues(vector<int> state);
	float getQValue(vector<int> state, int action);
	void setQValue(vector<int> state, int action, float qvalue);
	int getBestAction(vector<int> state);
	float getMaxQValue(vector<int> state);
private:
	int actionNum;
};

