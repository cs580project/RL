#pragma once
#include "RLPolicy.h"
#include "RLWorld.h"
enum LearningMethod
{
	Q_LEARNING,
	SARSA,
	Q_LAMBDA
};

enum SelectActionMethod
{
	E_GREEDY,
	SOFTMAX
};

class RLearner
{
public:
	
	RLearner();
	~RLearner();

	void runEpoch();
	bool running;
private:
	RLPolicy policy;
	RLWorld theWorld;
	vector<int> currentState;
	LearningMethod learningMethod;
	void QLearning();
	void Sarsa();
	void QLambda();

	SelectActionMethod selectActionMethod;
	int selectAction(vector<int>& state);

	float alpha;
	float gamma;
};

