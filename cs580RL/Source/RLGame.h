#pragma once
#include "statemch.h"
#include "RLWorld.h"
#include "RLearner.h"


enum RLSpeed
{
    SuperSlow = 0,
    Slow,
    Medium,
    Fast,
    Turbo
};


class RLGame : public StateMachine
{

public:
    RLGame(GameObject & object);
	~RLGame(){}

private:
    virtual bool States(State_Machine_Event event, MSG_Object * msg, int state, int substate);

	RLearner        m_RLearner;
	RLWorld         m_learningWorld;

    float           m_punishmentValue;
    float           m_rewardValue;
    LearningMethod  m_learningMethod;
    int             m_trainingIterations;
    int             m_iterationsPerFrame;

};

