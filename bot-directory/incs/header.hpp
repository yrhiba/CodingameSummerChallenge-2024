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
#include <sstream>
using namespace std;

# define PRINT_INPUT 0
# define TEST_ONE_TURN 0
# define DEBUG 0

const string directions[8] {"D","R","U","L","DR","DL","TL","TR"};
const int dr[8] {1, 0, -1, 0, 1, 1, -1, -1};
const int dc[8] {0, 1, 0, -1, 1, -1, -1, 1};

# define all(x) begin(x), end(x)
# define rall(x) rbegin(x), rend(x)

int player_idx, nb_games, turn = 0;

struct State;

struct Data // updated data calculated each turn
{
	int maxHurdleTurns, minHurdleTurns, hurdleGarantideWin; // my player
	int hurdle_players_maxTurns[3]; // max turns needed for each player to reach the end
	int hurdle_players_minTurns[3]; // min turns needed for each player to reach the end
	int hurdle_players_garantide_win[3]; // 0: losing, 1: wining, -1: don't know
	int hurdle_players_garantide_lose[3]; // 0: losing, 1: wining, -1: don't know
	int hurdle_players_ranking_position[3]; // 0:gold, 1:silver, 2:bronze
	bool hurdle_game_over;

	float archery_players_bestDis[3];
	float archery_actions_bestDis[4];
	float archery_players_worstDis[3];
	int archery_players_garantide_win[3];
	int archery_playrs_garantie_lose[3];
	int archery_rest_turns;
	bool archery_game_over;

	int maxDivingScore, minDivingScore, divingNeedScore; // my player
	int diving_players_maxScores[3];
	int diving_players_minScores[3];
	bool diving_players_garantide_win[3];
	bool diving_players_garantide_lose[3];
	int diving_players_ranking_position[3];
	bool diving_game_over;

	/* todo seperatly */
	int hurdle_players_final_scores[3];
	int archery_players_final_scores[3];
	int diving_players_final_scores[3];

	void printHurdleData()
	{
		cerr << "game-over: " << hurdle_game_over << endl;

		for (int i = 0; i < 3; i++)
			cerr << "p" << i << ": maxH: " << hurdle_players_maxTurns[i] << ", minH: " << hurdle_players_minTurns[i] << endl;

		for (int i = 0; i < 3; i++)
			cerr << "p" << i << ": garantieWin:" << hurdle_players_garantide_win[i] << ", ";
		cerr << endl;

		for (int i = 0; i < 3; i++)
			cerr << "p" << i << ": rank: " << hurdle_players_ranking_position[i] << ", ";
		cerr << endl;
	}

	void printArcheryData()
	{
		cerr << "rest-turns: " << archery_rest_turns << ", game-over: " << archery_game_over << endl;

		for (int i = 0; i < 3; i++)
		{
			cerr << "p" << i << ": bestDis:" << archery_players_bestDis[i] << ", worstDis:" << archery_players_worstDis[i] << endl;
		}

		for (int i = 0; i < 3; i++)
		{
			cerr << "p" << i << ": gw:" << archery_players_garantide_win[i] << ", gl:" << archery_playrs_garantie_lose[i] << endl;
		}

		for (int d = 0; d < 4; d++)
		{
			cerr << "d: " << directions[d] << " best: " << archery_actions_bestDis[d] << endl;
		}
	}

	void printDivingData()
	{
		cerr << "game-over: " << diving_game_over << ", need-score: " << divingNeedScore << endl;
		for (int i = 0; i < 3; i++)
		{
			cerr << "minS: " << diving_players_minScores[i] << ", maxS: " << diving_players_maxScores[i] << endl;
		}
		for (int i = 0; i < 3; i++)
		{
			cerr << "gw: " << diving_players_garantide_win[i] << ", gl: " << diving_players_garantide_lose[i] << endl;
		}
		for (int i = 0; i < 3; i++)
		{
			cerr << "pr" << i << ": " << diving_players_ranking_position[i] << ", ";
		}
		cerr << endl;
	}

} _data;

void debug()
{
	// _data.printHurdleData();
	// _data.printArcheryData();
	// _data.printDivingData();
}

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

	void evaluateAndSetWeights()
	{
		float activeGames = (
			!_data.hurdle_game_over + 
			!_data.archery_game_over +
			!_data.diving_game_over
		);

		hurdle_score_weight = !_data.hurdle_game_over ? (1.0f / activeGames) : 0.0f;
		archery_score_weight = !_data.archery_game_over ? (1.0f / activeGames) : 0.0f;
		diving_score_weight = !_data.diving_game_over ? (1.0f / activeGames) : 0.0f;

		cerr << "active-games: " << activeGames << endl;
		cerr << "hw:" << hurdle_score_weight << " ";
		cerr << "aw:" << archery_score_weight << " ";
		cerr << "dw:" << diving_score_weight << endl;
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
