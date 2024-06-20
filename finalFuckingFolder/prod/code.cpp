/* yrhiba.github.io (c). 2022-2024 */

// ########################
// file-Name: incs/header.hpp
// ########################

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <cassert>
#include <random>
#include <cstdlib>
using namespace std;

# define all(x) begin(x), end(x)
# define rall(x) rbegin(x), rend(x)

int player_idx, nb_games;

// ########################
// file-Name: incs/state.hpp
// ########################

struct State
{
	int hurdle_turn;
	int hurdle_pos;
	int hurdle_stunned;
	int archery_x;
	int archery_y;
	int diving_point;
	int diving_combo;
	string hurdle_gpu;
	string diving_gpu;
	string archery_gpu;
};

ostream &operator<<(ostream &os, State &state)
{
	os << "hurdle-gpu: " << state.hurdle_gpu << endl;
	os << "archery-gpu: " << state.archery_gpu << endl;
	os << "diving-gpu: " << state.diving_gpu << endl;
	os << "hurdle: p:" << state.hurdle_pos;
	os << " s:" << state.hurdle_stunned;
	os << " t:" << state.hurdle_turn << endl;
	os << "archery: x:" << state.archery_x;
	os << " y:" << state.archery_y << endl;
	os << "diving: p:" << state.diving_point;
	os << " c:" << state.diving_combo << endl;
	return (os);
}
// ########################
// file-Name: src/game.cpp
// ########################

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
	state.diving_gpu.pop_back();
	if (state.diving_gpu.empty()) state.diving_gpu = "GAME_OVER";
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
// ########################
// file-Name: src/main.cpp
// ########################

void solve(State &state)
{
	cerr << state << endl;

	State nx = get_next_state(state, 'U');

	cerr << nx << endl;
}

int main()
{
	srand(time(0));
	cin >> player_idx >> nb_games;
	cin.ignore();
	while (true)
	{
		State state;
		for (int i = 0; i < 3; i++)
		{
			string score;
			getline(cin, score);
		}
		for (int i = 0; i < 4; i++)
		{
			string gpu;
			cin >> gpu;
			int regs[7];
			for (int r=0;r<7;r++)cin>>regs[r];
			cin.ignore();
			//
			if (i == 0)
			{
				state.hurdle_gpu = gpu;
				state.hurdle_pos = regs[player_idx];
				state.hurdle_stunned = regs[player_idx+3];
				state.hurdle_turn = 0;
			}
			else if (i == 1)
			{
				state.archery_gpu = gpu;
				state.archery_x = regs[player_idx*2];
				state.archery_y = regs[player_idx*2+1];
			}
			else if (i == 3)
			{
				state.diving_gpu = gpu;
				state.diving_point = regs[player_idx];
				state.diving_combo = regs[player_idx+3];
			}
		}
		reverse(rall(state.archery_gpu));
		reverse(rall(state.diving_gpu));
		solve(state);
		break;
	}
}
