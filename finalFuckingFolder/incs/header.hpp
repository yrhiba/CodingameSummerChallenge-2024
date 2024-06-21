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
#include <map>
using namespace std;

const string directions[8] {"D","R","U","L","DR","DL","TL","TR"};
const int dr[8] {1, 0, -1, 0, 1, 1, -1, -1};
const int dc[8] {0, 1, 0, -1, 1, -1, -1, 1};

# define all(x) begin(x), end(x)
# define rall(x) rbegin(x), rend(x)

int player_idx, nb_games;

struct Data
{
	int maxHurdleTurns;
	int minHurdleTurns;

	int maxDivingScore;
	int minDivingScore;

} _data;

struct Scoring
{
	/* evaluate importance of each mini-game */
	float hurdle_score_weight = 0.0f; /* 0 -> 100% */
	float archery_score_weight = 1.0f; /* 0 -> 100% */
	float diving_score_weight = 0.0f; /* 0 -> 100% */

	double finalScore(float hurdle_scroe, float archery_score, float diving_score)
	{
		hurdle_scroe *= hurdle_score_weight;
		archery_score *= archery_score_weight;
		diving_score *= diving_score_weight;
		return hurdle_scroe + archery_score + diving_score;
	}

} scoring;

/*end*/

/*game-utils*/
State get_next_state(const State &state, char action);

pair<float, bool> get_value_and_terminated(State &state);

string search(int number_of_iterations, State &state);

void precalcHurdleData(string &gpu, int pos, int stunned);

void precalcDivingData(string &gpu,  int point, int combo);
/*game-utils*/
