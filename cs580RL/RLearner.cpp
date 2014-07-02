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
	switch (learningMethod)
	{
	case Q_LEARNING:
		break;
	}
}
