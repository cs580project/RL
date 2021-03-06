/* Copyright Steve Rabin, 2012. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2012"
 */

//These message names are processed inside msg.h

REGISTER_MESSAGE_NAME(MSG_NULL)							//Reserved message name
REGISTER_MESSAGE_NAME(MSG_GENERIC_TIMER)				//Reserved message name
REGISTER_MESSAGE_NAME(MSG_SPAWN_GENERIC_TIMER)			//Reserved message name
REGISTER_MESSAGE_NAME(MSG_CHANGE_STATE_DELAYED)			//Reserved message name
REGISTER_MESSAGE_NAME(MSG_CHANGE_SUBSTATE_DELAYED)		//Reserved message name

//Add new messages here
REGISTER_MESSAGE_NAME(MSG_SetGoal)
REGISTER_MESSAGE_NAME(MSG_SetHeuristicWeight)
REGISTER_MESSAGE_NAME(MSG_SetHeuristicCalc)
REGISTER_MESSAGE_NAME(MSG_SetSmoothing)
REGISTER_MESSAGE_NAME(MSG_SetRubberbanding)
REGISTER_MESSAGE_NAME(MSG_SetStraightline)
REGISTER_MESSAGE_NAME(MSG_SetAgentSpeed)
REGISTER_MESSAGE_NAME(MSG_SetSingleStep)
REGISTER_MESSAGE_NAME(MSG_SetAStarUsesAnalysis)
REGISTER_MESSAGE_NAME(MSG_SetMoving)
REGISTER_MESSAGE_NAME(MSG_RunTimingsShort)
REGISTER_MESSAGE_NAME(MSG_RunTimingsLong)
REGISTER_MESSAGE_NAME(MSG_MapChange)
REGISTER_MESSAGE_NAME(MSG_SetFogOfWar)

//Used for Zombie and Human demo state machines
REGISTER_MESSAGE_NAME(MSG_CheckTouch)
REGISTER_MESSAGE_NAME(MSG_Tagged)
REGISTER_MESSAGE_NAME(MSG_AddNewWaypoint)
REGISTER_MESSAGE_NAME(MSG_MouseClick)
REGISTER_MESSAGE_NAME(MSG_Arrived)
REGISTER_MESSAGE_NAME(MSG_Reset)
REGISTER_MESSAGE_NAME(MSG_Teleport)


//Unit test messages
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage2)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage3)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage4)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage5)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage6)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage7)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage8)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage9)
REGISTER_MESSAGE_NAME(MSG_UnitTestBroken)
REGISTER_MESSAGE_NAME(MSG_UnitTestPing)
REGISTER_MESSAGE_NAME(MSG_UnitTestAck)
REGISTER_MESSAGE_NAME(MSG_UnitTestDone)
REGISTER_MESSAGE_NAME(MSG_UnitTestTimer)

//UI message(RL)
REGISTER_MESSAGE_NAME(MSG_SetPunish)
REGISTER_MESSAGE_NAME(MSG_SetReward)
REGISTER_MESSAGE_NAME(MSG_SetTrainLoop)
REGISTER_MESSAGE_NAME(MSG_SetMethod_UseQL)
REGISTER_MESSAGE_NAME(MSG_SetRLSpeed)
REGISTER_MESSAGE_NAME(MSG_StartLearning)
REGISTER_MESSAGE_NAME(MSG_StartPlaying)
REGISTER_MESSAGE_NAME(MSG_ResetLearner)
REGISTER_MESSAGE_NAME(MSG_ClearScores)
REGISTER_MESSAGE_NAME(MSG_SetPlayContinuous)
REGISTER_MESSAGE_NAME(MSG_UseSmartMouse)