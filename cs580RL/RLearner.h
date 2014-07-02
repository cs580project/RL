#pragma once
#include "RLPolicy.h"
#include "RLWorld.h"
enum LearningMethod
{
	Q_LEARNING
};
class RLearner
{
public:
	
	RLearner();
	~RLearner();

	void runEpoch();

private:
	RLPolicy policy;
	RLWorld theWorld;
	LearningMethod learningMethod;
};

