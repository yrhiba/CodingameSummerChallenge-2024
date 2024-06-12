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
		cin>>gpu;ign
		for (int r:regs)cin>>r;ign
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
		}ign
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
