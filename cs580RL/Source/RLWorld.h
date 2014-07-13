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

	float           GetReward();
	vector<int>&    GetNextState(int action, bool update);
	vector<int>&    GetCurrentState();
	void            ResetState();
	void            ResetScores();
    void            ResetAll();
    void            ResetAllButScores();

	bool            ValidAction(int action);	//check if the action is legal
    bool            EndState();
    void            DrawRLState(bool teleport);

    int  GetCatTrainingScore()      { return catScoresTraining;     };
    int  GetCatPlayingScore()       { return catScoresPlaying;      };
    int  GetMouseTrainingScore()    { return mouseScoresTraining;   };
    int  GetMousePlayingScore()     { return mouseScoresPlaying;    };

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
    void    UpdateScores(bool playing);
    bool    CatScored();
    bool    MouseScored();

	vector<int> stateArray;
	int     catScoresTraining, mouseScoresTraining;	//keep the score
    int     catScoresPlaying, mouseScoresPlaying;	//keep the score
	vector<int> currentState;	//include coords for mouse,cat,cheese

	int     mx, my;	            //coordinates for mouse
	int     cx, cy;             //coord for cat
	int     chx, chy;	        // coord for cheese

    int     mapWidth;

	float   waitingReward;
	float   currentReward, currentPunish;
};

