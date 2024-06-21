#pragma once

/*start*/

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <cassert>
#include <random>
#include <cstdlib>
#include <chrono>
#include <cmath>
using namespace std;

# define all(x) begin(x), end(x)
# define rall(x) rbegin(x), rend(x)

int player_idx, nb_games;

struct Scoring
{
	/* 100% priotizy */
	float hurdle_score_percent; /* 0 -> 100% */
	float archery_score_percent; /* 0 -> 100% */
	float diving_score_percent; /* 0 -> 100% */

	/* mini-game: scoring 0 -> 1 | */
	/*
		hurdle: 0 -> 1
	*/
	/*
		archery: 0 -> 1
	*/
	/*
		diving: 0 -> 1
	*/

	double finalScore(float hurdle_scroe, float archery_score, float diving_score)
	{
		hurdle_scroe *= hurdle_score_percent;
		archery_score *= archery_score_percent;
		diving_score *= diving_score_percent;
		return hurdle_scroe + archery_score + diving_score;
	}
};

/*end*/

/*game-utils*/
State get_next_state(const State &state, char action);

pair<float, bool> get_value_and_terminated(State &state);

string search(int number_of_iterations, State &state);
