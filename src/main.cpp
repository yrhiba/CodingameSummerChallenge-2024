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
		readTurnInput(game);

		vector<pair<int, string>> archeryActionOrder
			= getArcheryBestActionOrder(game.minGames[1]);

		cout << archeryActionOrder.front().second << endl;
	}
}
