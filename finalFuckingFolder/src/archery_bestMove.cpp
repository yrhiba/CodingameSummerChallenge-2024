#include "header.hpp"
#include "state.hpp"
#include <map>
#include <vector>

/*start*/
float archery_rec(float archery_dp[41][41][16], string &gpu, int x, int y, int i)
{
	if (x < -20) x = 20;
	if (y < -20) y = -20;
	if (x > 20) x = 20;
	if (y > 20) y = 20;
	if (i >= gpu.size()) return sqrt(x*x + y*y);
	if (archery_dp[x+20][y+20][i] != -1) return archery_dp[x+20][y+20][i];
	float dis = 1e9;
	for (int d = 0; d < 4; d++)
	{
		int wind = gpu[i] - '0';
		float curDis = archery_rec(archery_dp, gpu, x + (dc[d] * wind), y + (dr[d] * wind), i+1);
		if (curDis < dis) dis = curDis;
	}
	return archery_dp[x+20][y+20][i] = dis;
}

string archery_getBestMove(string gpu, int x, int y)
{
	reverse(all(gpu));

	if (gpu.empty() || gpu == "GAME_OVER") return "UP";

	float archery_dp[41][41][16];
	for (int i = 0; i < 41; i++)
		for (int j = 0; j < 41; j++)
			for (int k = 0; k < 16; k++) archery_dp[i][j][k] = -1;

	char bestAction = '\0';
	float dis = 1e9;
	for (int d = 0; d < 4; d++)
	{
		int wind = gpu[0] - '0';
		float curDis = archery_rec(archery_dp, gpu, x + (dc[d] * wind), y + (dr[d] * wind), 1);
		if (curDis < dis)
		{
			bestAction = directions[d][0];
			dis = curDis;
		}
	}

	if (bestAction == 'U') return "UP";
	else if (bestAction == 'D') return "DOWN";
	else if (bestAction == 'L') return "LEFT";
	else if (bestAction == 'R') return "RIGHT";

	return ("UP");
}
