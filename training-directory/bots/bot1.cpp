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
// file-Name: src/archery_bestMove.cpp
// ########################
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

pair<int, int> getMaxAndMinHurdleTurns(string &gpu, int pos, int stunned)
{
	if (gpu == "GAME_OVER") return {1e5, 1e5};

	pair<int, int> result;
	pair<int, int> cur;
	// calculating the max turn
	result.first = 0;
	cur = {pos, stunned};
	for (; cur.first < 29; )
	{
		result.first += 1;
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
	// calculating the min turns
	result.second = 0;
	cur = {pos, stunned};
	for (; cur.first < 29; )
	{
		result.second += 1;
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
	return (result);
}

pair<int, int> getMaxAndMinDivingScores(string &gpu,  int point, int combo)
{
	if (gpu == "GAME_OVER") return {1e5, 1e5};

	int maxDivingScore = point;

	for (char c : gpu)
	{
		combo += 1;
		maxDivingScore += combo;
	}

	return {maxDivingScore, point};
}
// ########################
// file-Name: src/garantide.cpp
// ########################

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
			_data.hurdle_players_minTurns[i] > _data.hurdle_players_maxTurns[(i+1)%3]
			&&
			_data.hurdle_players_minTurns[i] > _data.hurdle_players_maxTurns[(i+2)%3]
		);

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

	// if (_data.hurdleGarantideWin)
	// {
	// 	_data.hurdle_game_over = true;
	// 	return ;
	// }
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
	if (gpu == "GAME_OVER")
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
	for (int i = 0; i < 3; i++)
	{
		_data.archery_players_garantide_win[i] = (
			_data.archery_players_worstDis[i] > _data.archery_players_bestDis[(i+1)%3]
			&&
			_data.archery_players_worstDis[i] > _data.archery_players_bestDis[(i+2)%3]
		);
	}

	// if (_data.archery_players_garantide_win[player_idx])
	// {
	// 	_data.archery_game_over = true;
	// 	return ;
	// }
}

void update_diving_data(string &gpu, vector<int> &regs)
{
	if (gpu == "GAME_OVER")
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
	}

	_data.needScore = max(_data.diving_players_maxScores[(player_idx+1)%3],
		_data.diving_players_maxScores[(player_idx+2)%3]);

	// if (_data.diving_players_minScores[player_idx] > _data.needScore)
	// {
	// 	_data.diving_game_over = true;
	// 	return ;
	// }
	// if (_data.diving_players_maxScores[player_idx] < _data.needScore)
	// {
	// 	_data.diving_game_over = true;
	// 	return ;
	// }
}
// ########################
// file-Name: src/scoring.cpp
// ########################
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

	if (!_data.hurdle_game_over && (_data.maxHurdleTurns - _data.minHurdleTurns) != 0)
	{
		hurlde_score = ((_data.maxHurdleTurns - state.hurdle_turn) * 1.0f)
				/ ((_data.maxHurdleTurns - _data.minHurdleTurns) * 1.0f);
	}
	else hurlde_score = 0.0f;

	double archery_distance = state.archery_x * state.archery_x + 
		state.archery_y * state.archery_y;

	double archery_score;

	if (!_data.archery_game_over)
	{
		archery_score = 1.0f - (sqrt(archery_distance * 1.0f)
				/ sqrt(20.0f * 20.0f + 20.0f * 20.0f));
	}
	else archery_score = 0.0f;

	/*
		diving notes:
			- maxDiving convert it to maxNeeded.
	*/
	double diving_score;

	if (!_data.diving_game_over && (_data.maxDivingScore - _data.minDivingScore) != 0)
	{
		diving_score = 1.0f - (((_data.maxDivingScore - state.diving_point) * 1.0f)
			/ ((_data.maxDivingScore - _data.minDivingScore) * 1.0f));
	}
	else diving_score = 0.0f;

	double resultStateScore = scoring.finalScore(hurlde_score, archery_score, diving_score);

	return {resultStateScore, true};
}
// ########################
// file-Name: src/search.cpp
// ########################

struct Node
{
	State state;
	char action_taken;
	string expandable_moves = "UDLR";
	Node *parent;
	vector<Node*> childrens;
	int visit_count;
	float value_sum;
};

Node *newNode(const State &_state,
		Node *_parent = nullptr,
		char _action_taken = '\0')
{
	Node *node = new Node();
	node->state = _state;
	node->parent = _parent;
	node->action_taken = _action_taken;
	node->visit_count = 0;
	node->value_sum = 0;
	node->expandable_moves = "UDLR";
	return (node);
}

bool is_fully_expanded(Node *node)
{
	return (node->expandable_moves.empty() && node->childrens.size() > 0);
}

float get_ucb(Node *parent, Node *child)
{
	float qv = (child->value_sum / (child->visit_count * 1.0f));
	return qv + (1.41f * (sqrt(log(parent->visit_count)) / (child->visit_count * 1.0f)));
}

Node *select(Node *node)
{
	Node *ans = nullptr;
	double ucb = -1e9;
	for (auto i : node->childrens)
	{
		double curUcb = get_ucb(node, i);
		if (curUcb > ucb)
		{
			ucb = curUcb;
			ans = i;
		}
	}
	return (ans);
}

Node *expand(Node *node)
{
	char action = node->expandable_moves.back();
	node->expandable_moves.pop_back();
	//
	State childState = get_next_state(node->state, action);
	node->childrens.push_back(newNode(childState, node, action));
	return (node->childrens.back());
}

double simulate(Node *node)
{
	State curState = node->state;
	while (true)
	{
		pair<float, bool> x = get_value_and_terminated(curState);
		if (x.second) return (x.first);
		curState = get_next_state(curState, "UDLR"[rand() % 4]);
	}
	return (0.0f);
}

void backpropagate(Node *node, float value)
{
	while (node)
	{
		node->value_sum += value;
		node->visit_count += 1;
		node = node->parent;
	}
}

string search(int number_of_iterations, State &state)
{
	std::chrono::high_resolution_clock::time_point start_time, end_time;
	start_time = std::chrono::high_resolution_clock::now();
	//
	Node *root = newNode(state);
	for (int i = 0; i < number_of_iterations; i++)
	{
		Node *it = root;
		while (is_fully_expanded(it))
			it = select(it);
		pair<float, bool> x = get_value_and_terminated(it->state);
		float value = x.first;
		bool is_terminated = x.second;
		if (!is_terminated)
		{
			it = expand(it);
			value = simulate(it);
		}
		backpropagate(it, value);
		//
		end_time = std::chrono::high_resolution_clock::now();
		auto duration 
			= std::chrono::duration_cast<std::chrono::milliseconds>
				(end_time - start_time).count();
		//
		if (duration > 25)
		{
			cerr << "breaking-at-duration: " << duration << ", and-iterations: " << i+1 << endl;
			break;
		}
	}
	//
	char bestAction = 'U';
	float prob = -1e9;
	for (auto i : root->childrens)
	{
		float cp = (i->visit_count * 1.0f) / (root->visit_count * 1.0f);
		if (cp > prob)
		{
			prob = cp;
			bestAction = i->action_taken;
		}
	}
	//
	if (bestAction == 'U') return "UP";
	else if (bestAction == 'D') return "DOWN";
	else if (bestAction == 'L') return "LEFT";
	else if (bestAction == 'R') return "RIGHT";
	//
	return ("");
}
// ########################
// file-Name: src/main.cpp
// ########################

void solve(State &state)
{
	/* evaluate weights */

	/* take an action based on mcts or best-move */

	string action;

	if (scoring.archery_score_weight == 1.0f)
	{
		action = archery_getBestMove(
				state.archery_gpu,
				state.archery_x,
				state.archery_y
			);
	}
	else
	{
		action = search(10000, state);
	}

	cout << action << endl;
}

int main()
{
	cin.tie(0)->sync_with_stdio(0);
	srand(time(0));

	cin >> player_idx >> nb_games;
	cin.ignore();

	while (true)
	{
		State state;
		vector<string> scores(3);
		for (int i = 0; i < 3; i++)
		{
			getline(cin, scores[i]);
		}
		for (int i = 0; i < 4; i++)
		{
			string gpu;
			cin >> gpu;
			vector<int> regs(7);
			for (int r=0;r<7;r++)cin>>regs[r];
			cin.ignore();
			//
			if (i == 0)
			{
				state.hurdle_gpu = gpu;
				state.hurdle_pos = regs[player_idx];
				state.hurdle_stunned = regs[player_idx+3];
				state.hurdle_turn = 0;
				update_hurdle_data(gpu, regs);
			}
			else if (i == 1)
			{
				state.archery_gpu = gpu;
				state.archery_x = regs[player_idx*2];
				state.archery_y = regs[player_idx*2+1];
				update_archery_data(gpu, regs);
			}
			else if (i == 3)
			{
				state.diving_gpu = gpu;
				state.diving_point = regs[player_idx];
				state.diving_combo = regs[player_idx+3];
				update_diving_data(gpu, regs);
			}
		}
		reverse(rall(state.archery_gpu));
		reverse(rall(state.diving_gpu));
		scoring.evaluate();
		solve(state);
	}
}

