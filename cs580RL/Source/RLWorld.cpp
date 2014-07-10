#include "DXUT.h"
#include "RLWorld.h"
#include "terrain.h"
#include "global.h"
#include "database.h"


RLWorld::RLWorld()
{
	InitialValue();
	LoadMapInfo();
	ResetState();
}

RLWorld::~RLWorld()
{

}

float RLWorld::GetReward()
{
	return waitingReward;
}

vector<int>& RLWorld::GetCurrentState()
{
	stateArray[0] = mx;
	stateArray[1] = my;
	stateArray[2] = cx;
	stateArray[3] = cy;
	stateArray[4] = chx;
	stateArray[5] = chy;

	return stateArray;
}

vector<int>& RLWorld::GetNextState(int action)
{
	// action is mouse action:  0=u 1=ur 2=r 3=dr ... 7=ul
	Pos nPos = GetCoords(action);

	if (!IsWall(nPos.x, nPos.y))
    {
		// move agent
		mx = nPos.x;
		my = nPos.y;
	}
	else
	{
		//illegal action, do something
	}

	// update world
	MoveCat();
	
    // calculate reward
    waitingReward = CalcReward();

	// if mouse has cheese, relocate cheese
	if ((mx == chx) && (my == chy))
    {
	    Pos d = GetRandomPos();
	    chx = d.x;
	    chy = d.y;
	}

	/*// if cat has mouse, relocate mouse
	if ((mx==cx) && (my==cy)) {
	d = getRandomPos();
	mx = d.width;
	my = d.height;
	}*/

	return GetCurrentState();
}

void RLWorld::ResetState()
{
	waitingReward = 0;
	SetRandomPos();
}

void RLWorld::ResetGame()
{
	catScores   = 0;
	mouseScores = 0;
}
bool RLWorld::ValidAction(int action)
{
    Pos nPos;
    
    nPos = GetCoords(action);

    return (!IsWall(nPos.x, nPos.y));
}

bool RLWorld::EndState()
{
	return ((cx == mx) && (cy == my));
}

float RLWorld::CalcReward()
{
	float newReward = 0;

	if ((mx == chx) && (my == chy))
	{
		mouseScores++;
		newReward += currentReward;
	}

	if ((cx == mx) && (cy == my))
	{
		catScores++;
		newReward -= currentPunish;
	}

	return newReward;
}

bool RLWorld::IsWall(int a, int b)
{
    if ((a < 0) || (b < 0) ||
        (a >= g_terrain.GetWidth()) || (b >= g_terrain.GetWidth()))
    {
        return true;
    }

	bool returnVal = g_terrain.IsWall(a, b);

	return returnVal;
}

Pos RLWorld::GetRandomPos()
{
	Pos nPos;

	while (true)
	{
		nPos.x = (int)(rand() % g_terrain.GetWidth());
		nPos.y = (int)(rand() % g_terrain.GetWidth());

        if (!IsWall(nPos.x, nPos.y))
        {
            return nPos;
        }
	}
}

void RLWorld::MoveCat()
{
	Pos nPos;

	nPos    = MoveToNewPos(cx, cy, mx, my);
	cx      = nPos.x;
	cy      = nPos.y;
}

void RLWorld::MoveMouse()
{
	Pos nPos;

	nPos    = MoveToNewPos(mx, my, chx, chy);
	mx      = nPos.x;
	my      = nPos.y;
}

Pos RLWorld::MoveToNewPos(int currentx, int currenty, int targetx, int targety)
{
	Pos nPos(currentx, currenty);

    if (targetx == currentx)
    {
        nPos.x = currentx;
    }
    else
    {
        nPos.x += (targetx - currentx) / abs(targetx - currentx); // +/- 1 or 0
    }

    if (targety == currenty)
    {
        nPos.y = currenty;
    }
    else
    {
        nPos.y += (targety - currenty) / abs(targety - currenty); // +/- 1 or 0
    }

	// check if move legal	
    if (!IsWall(nPos.x, nPos.y))
    {
        return nPos;
    }

	// not legal, make random move
    static const int maxRandom = 20000;
    int limit = 0;
	while (limit < maxRandom)
	{
		nPos.x = currentx;
		nPos.y = currenty;

        // get a random movement from current position
		nPos.x += 1 - (int)(rand() % 3);
		nPos.y += 1 - (int)(rand() % 3);

        // then check to see if it's valid
        if (!IsWall(nPos.x, nPos.y))
        {
            return nPos;
        }
        else
        {
            ++limit;
        }
	}

    // randomization failed
    nPos.x = currentx;
    nPos.y = currenty;

    return nPos;
}

void RLWorld::LoadMapInfo()
{
	mapWidth = g_terrain.GetWidth();
}

void RLWorld::SetRandomPos()
{
    Pos nPos;
    
    nPos    = GetRandomPos();
	cx      = nPos.x;
	cy      = nPos.y;

    static const int maxRandom = 20000;
    int limit = 0;
    while (limit < maxRandom)
	{
		nPos = GetRandomPos();
		
        if ((nPos.x != cx) || (nPos.y != cy))
        {
            break;
        }
	}

	mx = nPos.x;
	my = nPos.y;

    limit = 0;
    while (limit < maxRandom)
	{
		nPos = GetRandomPos();

        if ((nPos.x != cx || nPos.y != cy) && (nPos.x != mx || nPos.y != my))
        {
            break;
        }
	}

	chx = nPos.x;
	chy = nPos.y;
}

void RLWorld::InitialValue()
{
	//vector<int> stateArray;

    for (int i = 0; i < 6; ++i)
    {
        stateArray.push_back(-1);
    }

	currentReward   = 50;
	currentPunish   = 10;
	catScores       = 0;
	mouseScores     = 0;
}

Pos RLWorld::GetCoords(int action)
{
	Pos nPos;

	nPos.x = mx;
	nPos.y = my;

	switch (action)
    {
	    case 0:     nPos.y = my - 1; break;							//go S
	    case 1:     nPos.y = my - 1; nPos.x = mx + 1; break;		//go SE
	    case 2:     nPos.x = mx + 1; break;							//go E
	    case 3:     nPos.y = my + 1; nPos.x = mx + 1; break;		//go NE
	    case 4:     nPos.y = my + 1; break;							//go N
	    case 5:     nPos.y = my + 1; nPos.x = mx - 1; break;		//go NW
	    case 6:     nPos.x = mx - 1; break;							//go W
	    case 7:     nPos.y = my - 1; nPos.x = mx - 1; break;		//go SW
	    default:    break;
	}

	return nPos;
}

void RLWorld::DrawRLState(bool teleport)
{
    if (teleport)
    {
        //g_database.SendMsgFromSystem(MSG_Teleport, MSG_Data(D3DXVECTOR2(mx, my)));
        g_database.SendMsgFromSystem(10, MSG_Teleport, MSG_Data(D3DXVECTOR2(static_cast<float>(mx), static_cast<float>(my))));
        g_database.SendMsgFromSystem(11, MSG_Teleport, MSG_Data(D3DXVECTOR2(static_cast<float>(cx), static_cast<float>(cy))));
    }
    else
    {
        g_database.SendMsgFromSystem(10, MSG_AddNewWaypoint, MSG_Data(D3DXVECTOR2(static_cast<float>(mx), static_cast<float>(my))));
        g_database.SendMsgFromSystem(11, MSG_AddNewWaypoint, MSG_Data(D3DXVECTOR2(static_cast<float>(cx), static_cast<float>(cy))));
    }

    g_terrain.ResetColors();
	g_terrain.SetColor(chx,chy, DEBUG_COLOR_BLUE);
}

int RLWorld::returnCatScore()
{
	return catScores;
}

int RLWorld::returnMouseScore()
{
	return mouseScores;
}
