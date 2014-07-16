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
    virtual bool    States(State_Machine_Event event, MSG_Object * msg, int state, int substate);

    void            ChangeAgentSpeeds();

	RLearner        m_RLearner;

    bool            m_useSmartMouse;
    bool            m_playIsContinuous;
    float           m_punishmentValue;
    float           m_rewardValue;
    LearningMethod  m_learningMethod;
    int             m_trainingIterations;
    int             m_iterationsPerFrame;

};
