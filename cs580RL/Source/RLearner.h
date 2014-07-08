#pragma once
#include "RLPolicy.h"
#include "RLWorld.h"

enum LearningMethod
{
	Q_LEARNING,
	SARSA,
	Q_LAMBDA
};

enum SelectActionMethod
{
	E_GREEDY,
	SOFTMAX
};

class RLearner
{
public:
    RLearner() { };
	RLearner(RLWorld& world);
	~RLearner();

	void RunTraining(int numberOfEpochs);

    inline RLPolicy&    GetPolicy()                     { return m_policy;       };
    inline bool&        GetRunning()                    { return m_running;      };
    inline bool&        GetPlaying()                    { return m_playing;      };
    inline void         SetRunning(bool const& running) { m_running = running;   };
    inline void         SetPlaying(bool const& playing) { m_playing = playing;   };
	RLWorld&		    getWorld() { return m_learningWorld; }
	int                 SelectAction(vector<int>& state);

private:

    void                RunEpoch();
    void                QLearning();
    void                Sarsa();
    void                QLambda();

	RLPolicy            m_policy;
	RLWorld             m_learningWorld;
	vector<int>         m_currentState;
	LearningMethod      m_learningMethod;
	SelectActionMethod  selectActionMethod;

	float   m_alpha;
    float   m_gamma;
    bool    m_running;
    bool    m_playing;
};

