#include "header.hpp"
/*start*/

struct MiniGame
{
	string gpu;
	vector<int> regs = vector<int>(7);
};

struct Game
{
	int running = true;
	int player_idx;
	int nb_games;
	vector<string> playersScore = vector<string>(3);
	vector<MiniGame> minGames = vector<MiniGame>(4);
};

int main()
{
	Game game;
	cin>>game.player_idx>>game.nb_games;ign
	//
	while (game.running)
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

		cout << "UP" << endl;
	}
}
