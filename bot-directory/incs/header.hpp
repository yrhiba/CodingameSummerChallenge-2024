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
	int maxHurdleTurns, minHurdleTurns, hurdleGarantideWin; // my player
	int hurdle_players_maxTurns[3]; // max turns needed for each player to reach the end
	int hurdle_players_minTurns[3]; // min turns needed for each player to reach the end
	int hurdle_players_garantide_win[3]; // 0: losing, 1: wining, -1: don't know
	int hurdle_players_ranking_position[3]; // 0:gold, 1:silver, 2:bronze
	bool hurdle_game_over;

	float archery_players_bestDis[3];
	float archery_players_worstDis[3];
	int archery_players_garantide_win[3];
	int archery_rest_turns;
	bool archery_game_over;

	int maxDivingScore, minDivingScore, needScore; // my player
	int diving_players_maxScores[3];
	int diving_players_minScores[3];
	bool diving_players_garantide_win[3];
	int diving_players_ranking_position[3];
	bool diving_game_over;

	/* todo seperatly */
	int hurdle_players_final_scores[3];
	int archery_players_final_scores[3];
	int diving_players_final_scores[3];

} _data;

struct Scoring
{
	float hurdle_score_weight;
	float archery_score_weight;
	float diving_score_weight;

	double finalScore(float hurdle_scroe, float archery_score, float diving_score)
	{
		hurdle_scroe *= hurdle_score_weight;
		archery_score *= archery_score_weight;
		diving_score *= diving_score_weight;
		return hurdle_scroe + archery_score + diving_score;
	}

	void evaluate()
	{
		float activeGames = (
			!_data.hurdle_game_over + 
			!_data.archery_game_over +
			!_data.diving_game_over
		);

		hurdle_score_weight = !_data.hurdle_game_over ? (1.0f / activeGames) : 0.0f;
		archery_score_weight = !_data.archery_game_over ? (1.0f / activeGames) : 0.0f;
		diving_score_weight = !_data.diving_game_over ? (1.0f / activeGames) : 0.0f;

		// cerr << "active-games: " << activeGames << endl;
		// cerr << _data.hurdle_game_over << ' ' << _data.archery_game_over << ' ' << _data.diving_game_over << endl; 
		// cerr << hurdle_score_weight << " " << archery_score_weight << " " << diving_score_weight << endl;
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
pair<int, int> getMaxAndMinHurdleTurns(string &gpu, int pos, int stunned);

pair<int, int> getMaxAndMinDivingScores(string &gpu,  int point, int combo);
/*game-utils*/
