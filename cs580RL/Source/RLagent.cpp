
#include "DXUT.h"
#include "RLAgent.h"
#include "database.h"
#include "movement.h"
#include "terrain.h"
#include "body.h"



//Add new states below
enum StateName {
	STATE_Initialize,	//The first enum is the starting state
	STATE_Listening,
	STATE_Moving
};


//Add new substates below
enum SubstateName {		//Order doesn't matter
	//empty
};



bool RLAgent::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{

    BeginStateMachine


	//Global message response section
	OnMsg( MSG_Reset )
		ResetStateMachine();
        ChangeState(STATE_Initialize);

    OnMsg(MSG_Teleport)
        int r = msg->GetVector2Data().x; 
	    int c = msg->GetVector2Data().y;
        D3DXVECTOR3 coords = g_terrain.GetCoordinates(r, c);
        m_owner->GetBody().SetPos(coords);

// #define DEBUG_THE_RL_AGENT
#ifdef DEBUG_THE_RL_AGENT
        // TODO: Before making this player input code, check for IsWall
    OnMsg(MSG_MouseClick)
        int r = 0; int c = 0;
        g_terrain.GetRowColumn(&msg->GetVector3Data(), &r, &c);
        MSG_Data data(D3DXVECTOR2(r, c));
        SendMsgToStateMachine(MSG_AddNewWaypoint, data);
#endif

	///////////////////////////////////////////////////////////////
    DeclareState(STATE_Initialize)

        OnEnter
            m_owner->GetMovement().ResetPath();
			ChangeState( STATE_Listening );
	

	///////////////////////////////////////////////////////////////
    DeclareState(STATE_Listening)

        OnEnter
            m_owner->GetMovement().SetIdleSpeed();

        OnMsg(MSG_AddNewWaypoint)
            int r = msg->GetVector2Data().x; 
	        int c = msg->GetVector2Data().y;
            m_owner->GetMovement().AddToPath(r, c);
            ChangeState(STATE_Moving);


	///////////////////////////////////////////////////////////////
    DeclareState(STATE_Moving)

        OnEnter
            m_owner->GetMovement().SetWalkSpeed();

        OnMsg(MSG_AddNewWaypoint)
            int r = msg->GetVector2Data().x; 
	        int c = msg->GetVector2Data().y;
            m_owner->GetMovement().AddToPath(r, c);

        OnMsg(MSG_Arrived)
            ChangeState(STATE_Listening);


    EndStateMachine

}

