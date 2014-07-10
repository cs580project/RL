

#pragma once

#include "statemch.h"


class RLAgent : public StateMachine
{
public:

	RLAgent( GameObject & object ) : StateMachine( object ), m_jogging( false ) {}
	~RLAgent( void ) {}

private:

	virtual bool States( State_Machine_Event event, MSG_Object * msg, int state, int substate );

    bool m_jogging;
};
