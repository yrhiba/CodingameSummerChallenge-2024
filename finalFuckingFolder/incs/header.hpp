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

struct State;

struct Data // updated data calculated each turn
{
	int maxHurdleTurns, minHurdleTurns; // my player
	int hurdle_players_maxTurns[3]; // max turns needed for each player to reach the end
	int hurdle_players_minTurns[3]; // min turns needed for each player to reach the end
	bool hurdle_players_garantide_win[3];
	int hurdle_players_ranking_position[3];
	int hurdle_players_final_scores[3];
	bool hurdle_game_over;

	bool archery_players_garantide_win[3];
	int archery_players_ranking_position[3];
	int archery_players_final_scores[3];
	bool archery_game_over;

	int maxDivingScore, minDivingScore; // my player
	int diving_players_maxScores[3];
	int diving_players_minScores[3];
	bool diving_players_garantide_win[3];
	int diving_players_ranking_position[3];
	int diving_players_final_scores[3];
	bool diving_game_over;

} _data;

struct Scoring
{
	float hurdle_score_weight = 0.3f;
	float archery_score_weight = 0.35f;
	float diving_score_weight = 0.35f;

	double finalScore(float hurdle_scroe, float archery_score, float diving_score)
	{
		hurdle_scroe *= hurdle_score_weight;
		archery_score *= archery_score_weight;
		diving_score *= diving_score_weight;
		return hurdle_scroe + archery_score + diving_score;
	}

	void evaluate(State &state)
	{
		/* evaluate hurdle */

		/* evaluate archery */

		/* evaluate diving */
	}

	void gamesCheckAndUpdateEmergency()
	{
	}

} scoring;

/*end*/

/*game-utils*/
State get_next_state(const State &state, char action);

pair<float, bool> get_value_and_terminated(State &state);

string search(int number_of_iterations, State &state);

/*
	return (maxTurns, minTurns)
*/
pair<int, int> precalcHurdleData(string &gpu, int pos, int stunned);

void precalcDivingData(string &gpu,  int point, int combo);
/*game-utils*/
