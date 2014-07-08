#include "DXUT.h"
#include "RLWorld.h"
#include "terrain.h"
#include "global.h"
#include "database.h"


RLWorld::RLWorld()
{
	InitialValue();
	loadMapInfo();
	resetState();
}


RLWorld::~RLWorld()
{
	if (currentMap)
	{
		//for (int i = 0; i < mapWidth; i++)
		//	delete currentMap[i];

		//delete[] currentMap;
	}
}

float RLWorld::getReward()
{
	return waitingReward;
}


vector<int>& RLWorld::getCurrentState()
{
	stateArray[0] = mx;
	stateArray[1] = my;
	stateArray[2] = cx;
	stateArray[3] = cy;
	stateArray[4] = chx;
	stateArray[5] = chy;

	return stateArray;
}

vector<int>& RLWorld::getNextState(int action)
{
	// action is mouse action:  0=u 1=ur 2=r 3=dr ... 7=ul
	Pos nPos = getCoords(action);

	if (!isWall(nPos.x, nPos.y)) {
		// move agent
		mx = nPos.x;
		my = nPos.y;
	}
	else
	{
		//illegal action, do something
	}
	// update world
	moveCat();
	waitingReward = calcReward();

	// if mouse has cheese, relocate cheese
	if ((mx == chx) && (my == chy)) {
	Pos d = getRandomPos();
	chx = d.x;
	chy = d.y;
	}

	/*// if cat has mouse, relocate mouse
	if ((mx==cx) && (my==cy)) {
	d = getRandomPos();
	mx = d.width;
	my = d.height;
	}*/

	return getCurrentState();
}

void RLWorld::resetState()
{
	waitingReward = 0;
	setRandomPos();
}

void RLWorld::resetGame()
{
	catScores = 0;
	mouseScores = 0;
}
bool RLWorld::validAction(int action)
{
	Pos nPos = getCoords(action);
	return !isWall(nPos.x, nPos.y);
}

bool RLWorld::endState()
{
	return ((cx == mx) && (cy == my));
}

double RLWorld::calcReward()
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

bool RLWorld::isWall(int a, int b)
{
	if (a < 0 || b < 0 || a >= g_terrain.GetWidth() || b >= g_terrain.GetWidth()) return true;

	bool returnVal = g_terrain.IsWall(a, b);

	return returnVal;
}

Pos RLWorld::getRandomPos()
{

	Pos nPos;

	while (true)
	{
		nPos.x = (int)(rand() % g_terrain.GetWidth());
		nPos.y = (int)(rand() % g_terrain.GetWidth());

		if (!isWall(nPos.x, nPos.y)) return nPos;
	}
}

void RLWorld::moveCat()
{
	Pos nPos;
	nPos = moveToNewPos(cx, cy, mx, my);
	cx = nPos.x;
	cy = nPos.y;
}

void RLWorld::moveMouse()
{
	Pos nPos;
	nPos = moveToNewPos(mx, my, chx, chy);
	mx = nPos.x;
	my = nPos.y;
}

Pos RLWorld::moveToNewPos(int currentx, int currenty, int targetx, int targety)
{
	Pos nPos(currentx, currenty);

	if (targetx == currentx) nPos.x = currentx;
	else nPos.x += (targetx - currentx) / abs(targetx - currentx); // +/- 1 or 0
	if (targety == currenty) nPos.y = currenty;
	else nPos.y += (targety - currenty) / abs(targety - currenty); // +/- 1 or 0

	// check if move legal	
	if (!isWall(nPos.x, nPos.y)) return nPos;

	// not legal, make random move
	while (true)
	{
		nPos.x = currentx;
		nPos.y = currenty;

		nPos.x += 1 - (int)(rand() % 3);		//get a random position then
		nPos.y += 1 - (int)(rand() % 3);

		if (!isWall(nPos.x, nPos.y)) return nPos;
	}
}

void RLWorld::loadMapInfo()
{
	mapWidth = g_terrain.GetWidth();

	//currentMap = new int*[mapWidth];

	//for (int i = 0; i < mapWidth; i++)
	//	currentMap[i] = new int[mapWidth];
}

void RLWorld::setRandomPos()
{
	Pos nPos = getRandomPos();
	cx = nPos.x;
	cy = nPos.y;

	while (true)
	{
		nPos = getRandomPos();
		if (nPos.x != cx || nPos.y != cy) break;
	}

	mx = nPos.x;
	my = nPos.y;

	while (true)
	{
		nPos = getRandomPos();
		if ((nPos.x != cx || nPos.y != cy) && (nPos.x != mx || nPos.y != my)) break;
	}

	chx = nPos.x;
	chy = nPos.y;
}

void RLWorld::InitialValue()
{
	//vector<int> stateArray;

	for (int i = 0; i < 6; i++)
		stateArray.push_back(-1);

	currentReward = 50;
	currentPunish = 10;
	catScores = 0;
	mouseScores = 0;
}

Pos RLWorld::getCoords(int action)
{
	Pos nPos;
	nPos.x = mx;
	nPos.y = my;

	switch (action) {
	case 0: nPos.y = my - 1; break;							//go S
	case 1: nPos.y = my - 1; nPos.x = mx + 1; break;		//go SE
	case 2: nPos.x = mx + 1; break;							//go E
	case 3: nPos.y = my + 1; nPos.x = mx + 1; break;		//go NE
	case 4: nPos.y = my + 1; break;							//go N
	case 5: nPos.y = my + 1; nPos.x = mx - 1; break;		//go NW
	case 6: nPos.x = mx - 1; break;							//go W
	case 7: nPos.y = my - 1; nPos.x = mx - 1; break;		//go SW
	default:
		break;
	}
	return nPos;
}

void RLWorld::drawState()
{
	//g_database.SendMsgFromSystem(MSG_Teleport, MSG_Data(D3DXVECTOR2(mx, my)));
	g_database.SendMsgFromSystem(10, MSG_Teleport, MSG_Data(D3DXVECTOR2(mx, my)));
	g_database.SendMsgFromSystem(11, MSG_Teleport, MSG_Data(D3DXVECTOR2(cx, cy)));
	//Sleep(200);
	;
	g_terrain.ResetColors();
	g_terrain.SetColor(chx,chy, DEBUG_COLOR_BLUE);
}


