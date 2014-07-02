#include "DXUT.h"
#include "database.h"
#include "RLGame.h"

void RLGame::init()
{
	theWorld = RLWorld();
	rLearner = RLearner(theWorld);
}

bool RLGame::States(State_Machine_Event event, MSG_Object * msg, int state, int substate)
{
	return true;
}

