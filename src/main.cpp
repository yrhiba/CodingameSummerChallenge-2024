#include "header.hpp"
/*start*/
int main()
{
	cin>>playerIdx>>nbGames;ign
	while(stillGameRunning)
	{
		// read turn data
		players.read(), minGames.read();

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
		}

		// final turn stage
		cout << "UP" << endl;
	}
}
