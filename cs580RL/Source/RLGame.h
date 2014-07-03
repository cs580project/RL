#pragma once
#include "statemch.h"
#include "RLWorld.h"
#include "RLearner.h"
class RLGame : public StateMachine
{

public:
	RLGame(GameObject & object)
		: StateMachine(object) {}
	~RLGame(){}

	void init();

private:
	RLearner rLearner;
	RLWorld theWorld;

	virtual bool States(State_Machine_Event event, MSG_Object * msg, int state, int substate);
};

