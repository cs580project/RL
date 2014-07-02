#include "DXUT.h"
#include "RLWorld.h"


RLWorld::RLWorld()
{
}


RLWorld::~RLWorld()
{
}

float RLWorld::getReward()
{
	return 0;
}


vector<int>& RLWorld::getNextState(int action)
{
	return vector<int>();
}

vector<int>& RLWorld::getCurrentState()
{
	return vector<int>();
}

void RLWorld::resetState()
{

}

bool RLWorld::validAction()
{
	return 0;
}

bool RLWorld::endState()
{
	return 0;
}
