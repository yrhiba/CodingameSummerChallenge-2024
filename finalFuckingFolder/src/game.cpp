#pragma once

#include "header.hpp"
#include "state.hpp"

/*start*/

void update_hurdle_race(State &state,  char action)
{
	if (state.hurdle_gpu == "GAME_OVER") return;

	bool finished = false;

	pair<int, bool> act;

	if (action == 'R') act = {3, false};
	else if (action == 'U') act = {2, true};
	else if (action == 'D') act = {2, false};
	else if (action == 'L') act = {1, false};

	if (state.hurdle_stunned) state.hurdle_stunned -= 1;
	else
	{
		for (int m = 1; m <= act.first; m++)
		{
			state.hurdle_pos = min(29, state.hurdle_pos+1);
			if (state.hurdle_gpu[state.hurdle_pos] == '#' 
				&& !act.second)
			{
				state.hurdle_stunned = 2;
				break;
			}
			if (state.hurdle_pos == 29)
			{
				finished = true;
				break;
			}
			act.second = false;
		}
	}
	//
	if (finished) state.hurdle_gpu = "GAME_OVER";
	//
	state.hurdle_turn += 1;
}

void update_archery(State &state, char action)
{
	if (state.archery_gpu == "GAME_OVER") return ;
	int offcet = state.archery_gpu.back() - '0';
	int dx = 0, dy = 0;
	if (action == 'D') dy = offcet;
	else if (action == 'L') dx = -offcet;
	else if (action == 'R') dx = offcet;
	else if (action == 'U') dy = -offcet;
	int &posx = state.archery_x;
	int &posy = state.archery_y;
	posx += dx;
	posy += dy;
	if (posx > 20) posx = 20;
	if (posx < -20) posx = -20;
	if (posy > 20) posy = 20;
	if (posy < -20) posy = -20;
	state.archery_gpu.pop_back();
	if (state.archery_gpu.empty()) state.archery_gpu = "GAME_OVER";
}

void update_diving(State &state, char action)
{
	if (state.diving_gpu == "GAME_OVER") return ;

	if (action == state.diving_gpu.back())
	{
		state.diving_combo += 1;
		state.diving_point += state.diving_combo;
	}
	else state.diving_combo = 0;
	//
	state.diving_gpu.pop_back();
	//
	if (state.diving_gpu.empty()) state.diving_gpu = "GAME_OVER";
}

State get_next_state(const State &state, char action)
{
	State nextState = state;
	update_hurdle_race(nextState, action);
	update_archery(nextState, action);
	update_diving(nextState, action);
	return (nextState);
}

pair<float, bool> get_value_and_terminated(State &state)
{
	int games_over = 0;
	games_over += (state.hurdle_gpu == "GAME_OVER");
	games_over += (state.diving_gpu == "GAME_OVER");
	games_over += (state.archery_gpu == "GAME_OVER");
	//
	if (games_over < 3)	return {0.0f, false};
	//
	double hurlde_score;

	if ((_data.maxHurdleTurns - _data.minHurdleTurns) != 0)
	{
		hurlde_score = ((_data.maxHurdleTurns - state.hurdle_turn) * 1.0f)
				/ ((_data.maxHurdleTurns - _data.minHurdleTurns) * 1.0f);
	}
	else hurlde_score = 0.0f;

	double archery_distance = state.archery_x * state.archery_x + 
		state.archery_y * state.archery_y;

	double archery_score = 1.0f - (sqrt(archery_distance * 1.0f)
			/ sqrt(20.0f * 20.0f + 20.0f * 20.0f));

	double diving_score = 1.0f - (((_data.maxDivingScore - state.diving_point) * 1.0f)
		/ ((_data.maxDivingScore - _data.minDivingScore) * 1.0f));

	double resultStateScore = hurlde_score + archery_score + diving_score;

	return {resultStateScore / 3.0f, true};
}

void precalcHurdleData(string &gpu, int pos, int stunned)
{
	if (gpu == "GAME_OVER") return ;

	pair<int, int> cur;
	//
	_data.maxHurdleTurns = 0;
	cur = {pos, stunned};
	for (; cur.first < 29; )
	{
		_data.maxHurdleTurns += 1;
		if (cur.second) cur.second -= 1;
		else
		{
			cur.first += 1;
			if (cur.first < gpu.size() && gpu[cur.first] == '#')
			{
				cur.second = 2;
				continue;
			}
		}
	}
	//
	_data.minHurdleTurns = 0;
	cur = {pos, stunned};
	for (; cur.first < 29; )
	{
		_data.minHurdleTurns += 1;
		if (cur.second) cur.second -= 1;
		else
		{
			for (int m = 1; m <= 3; m++)
			{
				cur.first = min(29, cur.first + 1);
				if (cur.first == 29) break;
				if (cur.first < gpu.size() && gpu[cur.first] == '#')
				{
					if (m == 1) cur.first += 1;
					else if (m == 2) cur.first -= 1;
					else if (m == 3) cur.first -= 1;
					break;
				}
			}
		}
	}
}

void precalcDivingData(string &gpu,  int point, int combo)
{
	if (gpu == "GAME_OVER") return ;
	_data.maxDivingScore = point;
	for (char c : gpu)
	{
		combo += 1;
		_data.maxDivingScore += combo;
	}
	_data.minDivingScore = point;
}
