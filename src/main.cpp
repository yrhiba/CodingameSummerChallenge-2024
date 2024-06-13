#include "header.hpp"

/*start*/
void readTurnInput(Game &game)
{
	for (string &lineScore : game.playersScore)
	{
		getline(cin, lineScore);
	}
	for (int g = 0; g < game.nb_games; g++)
	{
		MiniGame &curGame = game.minGames[g];
		cin >> curGame.gpu;
		for (int &reg : curGame.regs) cin>>reg;ign
	}
}

int main()
{
	cin>>game.player_idx>>game.nb_games;ign
	//
	while (game.running)
	{
		game.turn += 1;
		readTurnInput(game);

		if (game.turn == 0)
		{
			if (game.minGames[0].gpu.substr(0, 4) == "....")
			{
				cout << "RIGHT" << endl;
				continue;
			}
		}


		cout << "UP" << endl;
	}
}
