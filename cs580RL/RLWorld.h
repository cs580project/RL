#pragma once
#include <vector>
using namespace std;
class RLWorld
{
public:
	RLWorld();
	~RLWorld();

	float getReward();
	void getWorldInfo();
	vector<int>& getNextState(int action);
	vector<int>& getCurrentState();
	void resetState();
	bool validAction();
	bool endState();
private:
	vector<int> currentState;
};

