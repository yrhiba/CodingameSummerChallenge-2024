#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <stack>
#include <utility>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <climits>
#include <cstdlib>
#include <numeric>
#include <cassert>
#include <chrono>
using namespace std;
/*macros*/
# define INF 1e9
# define MAXTURNS 1
/*globla-variables*/
const int nb_players = 3;
const int nb_games = 4;
string mcts_actions_list = "UDLR";
int player_idx;
vector<double> games_scoring {1, 0, 0};
/*state*/
/*state-struct*/
struct State
{
	int hurdle_pos, hurdle_stunnde, hurdle_turn;
	int archery_x, archery_y;
	int diving_point, diving_combo;
	vector<string> games_gpu;
};
ostream &operator<<(ostream &os, State &state)
{
	for (int i = 0; i < 3; i++)
		os << "gpu: " << state.games_gpu[i] << endl;
	os << "hurdle: ";
	os << "p:" << state.hurdle_pos << " s:" << state.hurdle_stunnde;
	os << " t:" << state.hurdle_turn << endl;
	os << "archery: x:";
	os << state.archery_x << " y:" << state.archery_y << endl;
	os << "diving: p:";
	os << state.diving_point << " c:" << state.diving_combo << endl;
	return (os);
}
/*game-utils*/
pair<double, bool> get_value_and_terminated(State &state)
{
	int games_over = 0;
	for (int game = 0; game < 3; game++)
		games_over += (state.games_gpu[game] == "GAME_OVER");
	if (games_over < 3)	return {0.0f, false};
	//
	double hurlde_score = 1.0f - (state.hurdle_turn / 30.0f);
	double archery_distance = state.archery_x*state.archery_x + 
		state.archery_y*state.archery_y;
	double archery_score = 1.0f - (archery_distance / (20.0f*20.0f + 20.0f*20.0f));
	double diving_score = (state.diving_point * 1.0f) / 120.0f;
	double resultStateScore = (
		hurlde_score * games_scoring[0]
		+ archery_score * games_scoring[1]
		+ diving_score * games_scoring[2]
	);
	// cout << state << endl;
	return {resultStateScore, true};
}

void update_hurdle_race(State &state,  char action)
{
	string &gpu = state.games_gpu[0];
	if (gpu == "GAME_OVER") return;
	bool finished = false;
	pair<int, bool> act;
	if (action == 'R') act = {3, false};
	else if (action == 'U') act = {2, true};
	else if (action == 'D') act = {2, false};
	else if (action == 'L') act = {1, false};
	int &pos = state.hurdle_pos;
	int &stunned = state.hurdle_stunnde;
	if (stunned) stunned -= 1;
	else
	{
		for (int m = 1; m <= act.first; m++)
		{
			pos = min(29, pos+1);
			if (gpu[pos] == '#' && !act.second)
			{
				stunned = 2;
				break;
			}
			if (pos == 29)
			{
				finished = true;
				break;
			}
			act.second = false;
		}
	}
	//
	state.hurdle_turn += 1;
	if (finished) gpu = "GAME_OVER";
}

void update_archery(State &state, char action)
{
	string &winds = state.games_gpu[1];
	if (winds == "GAME_OVER") return ;
	char act = action;
	int offcet = winds[0] - '0';
	int dx = 0;
	int dy = 0;
	if (act == 'D') dy = offcet;
	else if (act == 'L') dx = -offcet;
	else if (act == 'R') dx = offcet;
	else if (act == 'U') dy = -offcet;
	int &posx = state.archery_x;
	int &posy = state.archery_y;
	posx += dx;
	posy += dy;
	if (posx > 20) posx = 20;
	if (posx < -20) posx = -20;
	if (posy > 20) posy = 20;
	if (posy < -20) posy = -20;
	if (winds.size() > 1) winds = winds.substr(1, winds.size() - 1);
	else winds = "GAME_OVER";
}

void update_diving(State &state, char action)
{
	string &gpu = state.games_gpu[2];
	if (gpu == "GAME_OVER") return ;
	int &point = state.diving_point;
	int &combo = state.diving_combo;
	if (action == gpu[0])
	{
		combo += 1;
		point += combo;
	}
	else combo = 0;
	//
	if (gpu.size() > 1) gpu = gpu.substr(1, gpu.size() - 1);
	else gpu = "GAME_OVER";
}

State get_next_state(const State &state, char action)
{
	State nextState = state;
	update_hurdle_race(nextState, action);
	update_archery(nextState, action);
	update_diving(nextState, action);
	return (nextState);
}
/*mcts-class*/
struct Node
{
	State state;
	char action_taken;
	int expandable_index;
	Node *parent;
	vector<Node*> childrens;
	double visit_count;
	double value_sum;
	int depth;
};
Node *newNode(const State &_state,
		Node *_parent = nullptr,
		char _action_taken = '\0', int _depth = 0)
{
	Node *node = new Node();
	node->depth = _depth;
	node->state = _state;
	node->parent = _parent;
	node->action_taken = _action_taken;
	node->visit_count = 0;
	node->value_sum = 0;
	node->expandable_index = 0;
	return (node);
}

bool is_fully_expanded(Node *node)
{
	return (node->expandable_index >= 4 && node->childrens.size() > 0);
}

double get_ucb(Node *parent, Node *child)
{
	double qv = (child->value_sum / child->visit_count);
	return qv + (1.41f * (sqrt(parent->visit_count) / (child->visit_count * 1.0f)));
}

Node *select(Node *node)
{
	Node *ans = nullptr;
	double ucb = -INF;
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
	char action = mcts_actions_list[node->expandable_index++];
	//
	State childState = get_next_state(node->state, action);
	node->childrens.push_back(newNode(childState, node, action, node->depth + 1));
	return (node->childrens.back());
}

double simulate(Node *node)
{
	State curState = node->state;
	while (true)
	{
		auto x = get_value_and_terminated(curState);
		if (x.second) return (x.first);
		curState = get_next_state(curState, mcts_actions_list[rand() % 4]);
	}
	return (0.0f);
}

void backpropagate(Node *node, double value)
{
	node->value_sum += value;
	node->visit_count += 1;
	if (node->parent) backpropagate(node->parent, value);
}

void debug(Node *root)
{
	cerr << root->value_sum << " " << root->visit_count << endl;
	cerr << root->state << endl;

	for (auto i : root->childrens)
	{
		cerr << i->action_taken << " ";
		cerr << i->value_sum << " " << i->visit_count << endl;
		cerr << i->state << endl;
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
		
		pair<double, bool> x = get_value_and_terminated(it->state);
		double value = x.first;
		bool is_terminated = x.second;

		if (!is_terminated)
		{
			it = expand(it);
			value = simulate(it);
		}

		backpropagate(it, value);
	}
	//
	debug(root);
	//
	map<char, double> porb;
	int sum = 0;
	for (auto i : root->childrens)
	{
		porb[i->action_taken] = i->visit_count;
		sum += i->visit_count;
	}
	//
	char ansAct = 'X';
	float ansProb = -INF;
	for (auto [i, v] : porb)
	{
		v /= (sum * 1.0f);
		if (v > ansProb)
		{
			ansProb = v;
			ansAct = i;
		}
	}
	//
	map<char, string> mp;
	mp['U'] = "UP";
	mp['D'] = "DOWN";
	mp['L'] = "LEFT";
	mp['R'] = "RIGHT";
	//
	end_time = std::chrono::high_resolution_clock::now();
	auto duration 
		= std::chrono::duration_cast<std::chrono::milliseconds>
			(end_time - start_time).count();
	cerr << "duration: " << duration << endl;
	//
	return (mp[ansAct]);
}

void solve(State &state, int turn)
{
	cerr << state << endl;

	string action = search(5000, state);

	cout << action << endl;

	state = get_next_state(state, action[0]);

	cerr << state << endl;
}

int main()
{
	srand(time(0));
	cin >> player_idx;
	int nb_games, turn = 0;
	cin >> nb_games; cin.ignore();
	//
	int hurdle_race_turn = 0;
	//
	while (turn < MAXTURNS)
	{
		State state;
		state.games_gpu = vector<string>(3);
		for (int i = 0; i < 3; i++)
		{
			string score_info;
			getline(cin, score_info);
		}
		for (int i = 0; i < 4; i++)
		{
			string gpu;
			vector<int> regs(7);
			cin >> gpu;
			for (int r = 0; r < 7; r++) cin >> regs[r];
			cin.ignore();

			if (i == 0 || i == 1 || i == 3)
			{
				int idx = i;
				if (idx == 3) idx -= 1;

				if (i == 0)
				{
					if (gpu == "GAME_OVER") hurdle_race_turn = 0;
					else state.hurdle_turn = hurdle_race_turn++;

					state.hurdle_pos = regs[player_idx];
					state.hurdle_stunnde = regs[player_idx + 3];
				}
				else if (i == 1)
				{
					state.archery_x = regs[player_idx*2];
					state.archery_y = regs[player_idx*2+1];
				}
				else
				{
					state.diving_point = regs[player_idx];
					state.diving_combo = regs[player_idx+3];
				}
				//
				state.games_gpu[idx] = gpu;
			}
		}
		//
		solve(state, turn++);
	}
	//
	return (0);
}
