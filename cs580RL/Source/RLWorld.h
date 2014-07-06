#pragma once
#include <vector>
using namespace std;

struct Pos
{int x, y;};

class RLWorld
{
public:
	RLWorld();
	~RLWorld();

	vector<int> stateArray; //include coords for mouse,cat,cheese

	float getReward();
	vector<int>& getNextState(int action);
	vector<int>& getCurrentState();
	void resetState();
	bool validAction(int action);	//check if the action is legal
	bool endState();

	void setRewardVal(float reward) { currentReward = reward; }
	void setPunishVal(float punish) { currentPunish = punish; }

private:
	vector<int> currentState;	
	int mx, my;	//coordinates for mouse
	int cx, cy; //coord for cat
	int chx, chy;	// coord for cheese
	int catScores, mouseScores;	//keep the score
	double waitingReward;
	float currentReward, currentPunish;
	int** currentMap;	//map info
	int mapWidth;

	void loadMapInfo();
	double calcReward();	//calculate reward(waiting reward)

	bool isWall(int a, int b);
	
	Pos getRandomPos();
	void setRandomPos();

	void moveMouse();	//no learning
	void moveCat();

	void InitialValue();
	Pos moveToNewPos(int currentx, int currenty, int targetx, int targety);

	Pos getCoords(int action);
};

