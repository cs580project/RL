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
	vector<float>& getQValue(vector<int> state);
	void setQValue(vector<int> state, int acton, float qvalue);
	int getBestAction(vector<int> state);

private:
	int actionNum;
};

