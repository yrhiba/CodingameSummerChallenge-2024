#include "header.hpp"
/*start*/

vector<pair<int, string>> getArcheryBestActionOrder(MiniGame &archey)
{
	int colum = archey.regs[(game.player_idx * 2)];
	int row = archey.regs[(game.player_idx * 2) + 1];
	int wind = (archey.gpu[0] - '0');
	int isOver = (archey.gpu == "GAME_OVER");
	string ansAction = "";
	int	ansDistance = -1;
	vector<pair<int, string>> result;
	for (int d = 0; d < 4; d++)
	{
		int nr = row + (dr[d] * wind);
		int nc = colum + (dc[d] * wind);
		if (nc < -20) nc = -20;
		if (nc > 20) nc = 20;
		if (nr < -20) nr = -20;
		if (nr > 20) nr = 20;
		int currentDis = (nc * nc) + (nr * nr);
		result.push_back({currentDis, actions[d]});
		if ((ansDistance == -1) || (currentDis < ansDistance))
		{
			ansAction = actions[d];
			ansDistance = currentDis;
		}
	}
	sort(all(result));
	return (result);
}
