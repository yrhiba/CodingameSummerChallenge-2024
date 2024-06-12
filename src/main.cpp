#include "header.hpp"
/*start*/

int currentGame = 0, currentGold = 0;

int main()
{
	cin>>playerIdx;ign
	cin>>nbGames;ign
	while(stillGameRunning)
	{
		// read turn data
		players.read();
		minGames.read();

		// debug turn data
		cerr << "GameRound: " << turn++ << endl;

		Player &p = players.players[playerIdx];

		cerr << "totalScore: " << p.totalScore << "\n";
		for (int g=0;g<NBGAMES;g++)
		{
			gameScore &mg = p.scores[g];
			cerr << "Score of Game-" << g+1 << ": " << mg.getTotalScore() << " | ";
			cerr << "gold:" << mg.goldMedal << ", silver:" << mg.silverMedal;
			cerr << ", bronze:" << mg.bronzeMedal << "\n"; 
	
			HurdleRace &h = minGames.games[g];
			cerr << "gpu<" << h.gpu << ">." << endl;
			cerr << "regs: ";
			for (int r:h.regs) cerr << r << " ";
			cerr << endl;
		}

		// found next game
		if (players.players[playerIdx].scores[currentGame].goldMedal > currentGold)
		{
			currentGame += 1;
			currentGame %= nbGames;
			currentGold = players.players[playerIdx].scores[currentGame].goldMedal;
		};

		HurdleRace &curGame = minGames.games[currentGame];
		int playerCurPosition = minGames.games[currentGame].regs[playerIdx];

		// final turn stage
		cerr << "currentGame: " << currentGame << " " << playerCurPosition << endl;
		cout << minGames.games[currentGame].getBestAction(playerCurPosition) << endl;
	}
}
