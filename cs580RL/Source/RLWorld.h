#pragma once
#include <vector>
using namespace std;

struct Pos
{
	Pos() :x(0), y(0){ }
	Pos(int _x, int _y) :x(_x), y(_y){ }
	int x, y;
};

class RLWorld
{
public:
	RLWorld();
	~RLWorld();

    // TODO: Make these private.
    vector<int> stateArray;
    int catScores, mouseScores;	//keep the score

	float           GetReward();
	vector<int>&    GetNextState(int action);
	vector<int>&    GetCurrentState();
	void            ResetState();
	void            ResetGame();
	bool            ValidAction(int action);	//check if the action is legal
    bool            EndState();
    void            DrawRLState(bool teleport);

	void SetRewardVal(float reward) { currentReward = reward; }
	void SetPunishVal(float punish) { currentPunish = punish; }

private:
    void    InitialValue();
    void    LoadMapInfo();
    float   CalcReward();	    //calculate reward(waiting reward)
    bool    IsWall(int a, int b);
    Pos     MoveToNewPos(int currentx, int currenty, int targetx, int targety);
    Pos     GetCoords(int action);
    Pos     GetRandomPos();
    void    SetRandomPos();
    void    MoveMouse();	    //no learning
    void    MoveCat();

	vector<int> currentState;	//include coords for mouse,cat,cheese

	int     mx, my;	            //coordinates for mouse
	int     cx, cy;             //coord for cat
	int     chx, chy;	        // coord for cheese

    int**   currentMap;	        //map info
    int     mapWidth;

	float   waitingReward;
	float   currentReward, currentPunish;
};

