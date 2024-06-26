#include "header.hpp"
#include "state.hpp"
pair<int, int> getMaxAndMinHurdleTurns(string &gpu, int pos, int stunned);
pair<int, int> getMaxAndMinDivingScores(string &gpu,  int point, int combo);
/*start*/

void update_hurdle_data(string &gpu, vector<int> &regs)
{
	/* updating game over */
	if (gpu == "GAME_OVER" || regs[player_idx+3])
	{
		_data.hurdle_game_over = true;
		return ;
	}
	_data.hurdle_game_over = false;

	/* updating player min/max turns */
	for (int i = 0; i < 3; i++)
	{
		auto x = getMaxAndMinHurdleTurns(gpu, regs[i], regs[i+3]);
		_data.hurdle_players_maxTurns[i] = x.first;
		_data.hurdle_players_minTurns[i] = x.second;
	}

	for (int i = 0; i < 3; i++)
	{
		/* updating garantide win */
		_data.hurdle_players_garantide_win[i] = (
			_data.hurdle_players_maxTurns[i] <= _data.hurdle_players_minTurns[(i+1)%3]
			&&
			_data.hurdle_players_maxTurns[i] <= _data.hurdle_players_minTurns[(i+2)%3]
		);

		_data.hurdle_players_garantide_lose[i] = (
			_data.hurdle_players_minTurns[i] > _data.hurdle_players_maxTurns[(i+1)%3]
			&&
			_data.hurdle_players_minTurns[i] > _data.hurdle_players_maxTurns[(i+2)%3]
		);

		_data.hurdle_players_garantide_silver[i] = ((
			_data.hurdle_players_minTurns[i] > _data.hurdle_players_maxTurns[(i+1)%3]
			&&
			_data.hurdle_players_maxTurns[i] <= _data.hurdle_players_minTurns[(i+2)%3]
		) || (
			_data.hurdle_players_minTurns[i] > _data.hurdle_players_maxTurns[(i+2)%3]
			&&
			_data.hurdle_players_maxTurns[i] <= _data.hurdle_players_minTurns[(i+1)%3]
		));

		/* updating ranking positions */
		if (regs[i] >= regs[(i+1)%3] && regs[i] >= regs[(i+2)%3])
			_data.hurdle_players_ranking_position[i] = 0;
		else if (regs[i] >= regs[(i+1)%3] || regs[i] >= regs[(i+2)%3])
			_data.hurdle_players_ranking_position[i] = 1;
		else
			_data.hurdle_players_ranking_position[i] = 2;
	}

	// for fast access
	_data.maxHurdleTurns = _data.hurdle_players_maxTurns[player_idx];
	_data.minHurdleTurns = _data.hurdle_players_minTurns[player_idx];
	_data.hurdleGarantideWin = _data.hurdle_players_garantide_win[player_idx];

	if (_data.hurdle_players_garantide_win[player_idx]
		|| _data.hurdle_players_garantide_lose[player_idx]
		|| _data.hurdle_players_garantide_silver[player_idx]
		|| (turn + _data.hurdle_players_minTurns[player_idx]) > 100)
	{
		_data.hurdle_game_over = true;
		return ;
	}
}

float archeryMaxDpRec(
	float archery_dp[41][41][16], string &gpu, int x, int y, int i)
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
		float curDis = archeryMaxDpRec(archery_dp, gpu,
			x + (dc[d] * wind), y + (dr[d] * wind), i+1);
		if (curDis < dis) dis = curDis;
	}
	return archery_dp[x+20][y+20][i] = dis;
}

float archeryMinDpRec(
	float archery_dp[41][41][16], string &gpu, int x, int y, int i)
{
	if (x < -20) x = 20;
	if (y < -20) y = -20;
	if (x > 20) x = 20;
	if (y > 20) y = 20;
	if (i >= gpu.size()) return sqrt(x*x + y*y);
	if (archery_dp[x+20][y+20][i] != -1) return archery_dp[x+20][y+20][i];
	float dis = -1e9;
	for (int d = 0; d < 4; d++)
	{
		int wind = gpu[i] - '0';
		float curDis = archeryMinDpRec(archery_dp, gpu,
			x + (dc[d] * wind), y + (dr[d] * wind), i+1);
		if (curDis > dis) dis = curDis;
	}
	return archery_dp[x+20][y+20][i] = dis;
}

void update_archery_data(string &gpu, vector<int> &regs)
{
	if (gpu == "GAME_OVER" || (turn + gpu.size() > 100))
	{
		_data.archery_game_over = true;
		return ;
	}
	//
	_data.archery_game_over = false;
	_data.archery_rest_turns = gpu.size();
	//
	float archeryMaxDp[41][41][16];
	float archeryMinDp[41][41][16];
	for (int i = 0; i < 41; i++)
		for (int j = 0; j < 41; j++)
			for (int k = 0; k < 16; k++)
				archeryMaxDp[i][j][k] = archeryMinDp[i][j][k] = -1;
	//
	for (int i = 0; i < 3; i++)
	{
		_data.archery_players_bestDis[i] = archeryMaxDpRec(archeryMaxDp, gpu,
			regs[i*2], regs[i*2+1], 0);
		_data.archery_players_worstDis[i] = archeryMinDpRec(archeryMinDp, gpu,
			regs[i*2], regs[i*2+1], 0);
	}
	//
	int player_x = regs[player_idx*2];
	int player_y = regs[player_idx*2+1];
	int wind = (!gpu.empty() ? (gpu[0] - '0') : 0);
	for (int d = 0; d < 4; d++) // loop for each next actions
	{
		int nx = player_x + (dc[d] * wind);
		int ny = player_y + (dr[d] * wind);
		_data.archery_actions_bestDis[d] =  archeryMaxDpRec(archeryMaxDp, gpu,
			nx, ny, 1);
	}
	bool yesAllActionsHaveSameGoodDis = true;
	int garantiedWind = 0;
	for (int d = 0; d < 4; d++)
	{
		garantiedWind += (_data.archery_actions_bestDis[d]
			<= min(_data.archery_players_bestDis[(player_idx+1)%3],
				_data.archery_players_bestDis[(player_idx+2)%3])
		);
		//
		if (abs(_data.archery_actions_bestDis[d] - _data.archery_actions_bestDis[0]) > 1e-4)
		{
			yesAllActionsHaveSameGoodDis = false;
			break;
		}
	}
	//
	if (yesAllActionsHaveSameGoodDis || (garantiedWind >= 4))
	{
		_data.archery_game_over = true;
		return ;
	}
	//
	for (int i = 0; i < 3; i++)
	{
		_data.archery_players_garantide_win[i] = (
			_data.archery_players_worstDis[i] <= _data.archery_players_bestDis[(i+1)%3]
			&&
			_data.archery_players_worstDis[i] <= _data.archery_players_bestDis[(i+2)%3]
		);

		_data.archery_playrs_garantie_lose[i] = (
			_data.archery_players_bestDis[i] > _data.archery_players_worstDis[(i+1)%3]
			&&
			_data.archery_players_bestDis[i] > _data.archery_players_worstDis[(i+2)%3]
		);

		_data.archery_playrs_garantie_silver[i] = ((
			_data.archery_players_bestDis[i] > _data.archery_players_worstDis[(i+1)%3]
			&&
			_data.archery_players_worstDis[i] <= _data.archery_players_bestDis[(i+2)%3]
		) || (
			_data.archery_players_bestDis[i] > _data.archery_players_worstDis[(i+2)%3]
			&&
			_data.archery_players_worstDis[i] <= _data.archery_players_bestDis[(i+1)%3]
		));
	}

	if (_data.archery_players_garantide_win[player_idx]
		|| _data.archery_playrs_garantie_silver[player_idx]
		|| _data.archery_playrs_garantie_lose[player_idx])
	{
		_data.archery_game_over = true;
		return ;
	}
}

void update_diving_data(string &gpu, vector<int> &regs)
{
	if (gpu == "GAME_OVER" || (turn + gpu.size() > 100))
	{
		_data.diving_game_over = true;
		return ;
	}

	_data.diving_game_over = false;

	for (int i = 0; i < 3; i++)
	{
		auto x = getMaxAndMinDivingScores(gpu, regs[i], regs[i+3]);
		_data.diving_players_maxScores[i] = x.first;
		_data.diving_players_minScores[i] = x.second;
	}

	_data.maxDivingScore = _data.diving_players_maxScores[player_idx];
	_data.minDivingScore = _data.diving_players_minScores[player_idx];

	for (int i = 0; i < 3; i++)
	{
		_data.diving_players_garantide_win[i] = (
			_data.diving_players_minScores[i] > _data.diving_players_maxScores[(i+1)%3]
			&&
			_data.diving_players_minScores[i] > _data.diving_players_maxScores[(i+2)%3]
		);

		_data.diving_players_garantide_lose[i] = (
			_data.diving_players_maxScores[i] < _data.diving_players_minScores[(i+1)%3]
			&&
			_data.diving_players_maxScores[i] < _data.diving_players_minScores[(i+2)%3]
		);

		_data.diving_players_garantide_silver[i] = ((
			_data.diving_players_maxScores[i] < _data.diving_players_minScores[(i+1)%3]
			&&
			_data.diving_players_minScores[i] >= _data.diving_players_maxScores[(i+2)%3]
		) || (
			_data.diving_players_maxScores[i] < _data.diving_players_minScores[(i+2)%3]
			&&
			_data.diving_players_minScores[i] >= _data.diving_players_maxScores[(i+1)%3]
		));

		if (_data.diving_players_minScores[i] >= _data.diving_players_minScores[(i+1)%3]
			&& _data.diving_players_minScores[i] >= _data.diving_players_minScores[(i+2)%3])
		{
			_data.diving_players_ranking_position[i] = 0;
		}
		else if (_data.diving_players_minScores[i] >= _data.diving_players_minScores[(i+1)%3]
			|| _data.diving_players_minScores[i] >= _data.diving_players_minScores[(i+2)%3])
		{
			_data.diving_players_ranking_position[i] = 1;
		}
		else
			_data.diving_players_ranking_position[i] = 2;
	}

	_data.divingNeedScore = max(_data.diving_players_maxScores[(player_idx+1)%3],
		_data.diving_players_maxScores[(player_idx+2)%3]) - _data.diving_players_minScores[player_idx];

	_data.divingNeedScore = max(0, _data.divingNeedScore);

	if (_data.diving_players_garantide_win[player_idx]
		|| _data.diving_players_garantide_silver[player_idx]
		|| _data.diving_players_garantide_lose[player_idx])
	{
		_data.diving_game_over = true;
		return ;
	}
}
