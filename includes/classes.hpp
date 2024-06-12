#pragma once
#include "header.hpp"
/*start*/

struct HurdleRace
{
	// members
	string gpu;
	vector<int> regs = vector<int>(7);

	// methodes
	void read()
	{
		cin>>gpu;
		for (int &r:regs) cin>>r;
		ign
	}

	bool isValidMove(int pos, int movedBy)
	{
		bool jump = (movedBy == 2);
		for (int m = 1; m <= movedBy; m++)
		{
			if (pos + m >= gpu.size()) break;
			if ((gpu[pos+m] == '#') && !jump) return (false);
			jump = false;
		}
		return (true);
	}

	string getBestAction(int pos)
	{
		vector<pair<int, string>> actions = {
			{3, "RIGHT"},
			{2, "UP"},
			{1, "LEFT"}
		};
		for (auto [movedBy, action] : actions)
			if (isValidMove(pos, movedBy)) return action;
		return "none";
	}
};

struct gameScore
{
	int goldMedal,silverMedal,bronzeMedal;
	int getTotalScore(){return (goldMedal*3+silverMedal);}
};

struct Player
{
	// members
	int totalScore;
	vector<gameScore> scores = vector<gameScore>(NBGAMES);
	// methodes
	void read()
	{
		string line;
		getline(cin, line);
		stringstream S(line);
		S>>totalScore;
		for (int game = 0; game < NBGAMES; game++)
		{
			S>>scores[game].goldMedal;
			S>>scores[game].silverMedal;
			S>>scores[game].bronzeMedal;
		}
	}
};

struct Players
{
	// members
	vector<Player> players = vector<Player>(NBPLAYERS);

	// methodes
	void read(){for(int i=0;i<NBPLAYERS;i++)players[i].read();}
};

struct MiniGames
{
	// members
	vector<HurdleRace> games = vector<HurdleRace>(NBGAMES);

	// methodes
	void read(){for(int i=0;i<NBGAMES;i++)games[i].read();}
};
