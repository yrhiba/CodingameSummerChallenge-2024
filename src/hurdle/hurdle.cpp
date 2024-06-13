#include "header.hpp"
/*start*/

vector<pair<int, string>> getHurdleActionOrder(MiniGame &hurdle)
{
	vector<pair<int, string>> res;
	int myPos = hurdle.regs[game.player_idx];
	for (int d = 0; d < 4; d++)
	{
		int movedBy = mov[d].first;
		int jump = mov[d].second;
		int score = 0;
		//
		if ((hurdle.gpu != "GAME_OVER") && (hurdle.regs[game.player_idx+3] == 0))
			for (int j = 1; j <= movedBy && myPos+j<=hurdle.gpu.size(); j++)
			{
				if ((myPos+j)<hurdle.gpu.size() && !jump && hurdle.gpu[myPos+j] == '#')
				{
					score -= 2;
					break;
				}
				else score += 1;
				//
				jump = 0;
			}
		//
		res.push_back({score, actions[d]});
	}
	sort(rall(res));
	return (res);
}
