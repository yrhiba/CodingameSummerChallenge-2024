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
using namespace std;
/*macros*/
# define INF 1e9
/*globla-variables*/
const int nb_players = 3;
const int nb_games = 4;
vector<string> mcts_actions_list;
int player_idx;
/*utils-structers*/
struct Medals
{
	int gold;
	int silver;
	int bronze;
	Medals()
	{
		gold = 0;
		silver = 0;
		bronze = 0;
	}
};
istream &operator>>(istream &input_stream, Medals &medals)
{
	input_stream >> medals.gold >> medals.silver >> medals.bronze;
	return (input_stream);
}
ostream &operator<<(ostream &os, Medals &medals)
{
	os << "gold(" << medals.gold << "), ";
	os << "silver(" << medals.silver << "), ";
	os << "bronze(" << medals.bronze << ").";
	return (os);
}
/*state-struct*/
struct State
{
	vector<int> players_final_scores;
	vector<vector<Medals>> players_medals;
	vector<string> games_gpu;
	vector<vector<int>> games_regs;
	State()
	{
		players_final_scores = vector<int>(3, 0);
		players_medals = vector<vector<Medals>>(3, vector<Medals>(3, Medals()));
		games_gpu = vector<string>(3);
		games_regs = vector<vector<int>>(3, vector<int>(7, 0));
	}
	State(const State &other)
	{
		players_final_scores = other.players_final_scores;
		players_medals = other.players_medals;
		games_gpu = other.games_gpu;
		games_regs = other.games_regs;
	}
	State &operator=(const State &other)
	{
		players_final_scores = other.players_final_scores;
		players_medals = other.players_medals;
		games_gpu = other.games_gpu;
		games_regs = other.games_regs;
		return *this;
	}
};
ostream &operator<<(ostream &os, State &state)
{
	os << "---------------" << endl;
	os << "players-state-info: " << endl;
	for (int player = 0; player < 3; player++)
	{
		os << "player-" << player << ", final-score=(";
		os << state.players_final_scores[player] << ")." << endl;
		for (int game = 0; game < 3; game++)
		{
			os << "game-" << game << "-medals: ";
			os << state.players_medals[player][game] << endl;
		}
		os << "---------------" << endl;
	}
	os << "games-state-info: " << endl;
	os << "---------------" << endl;
	for (int game = 0; game < 3; game++)
	{
		os << "game-" << game << endl;
		os << "gpu: " << state.games_gpu[game] << endl;
		os << "regs: ";
		for (int r = 0; r < 7; r++)
		{
			os << state.games_regs[game][r] << " \n"[r+1 == 7];
		}
	}
	return (os);
}
/**/
/*game-utils*/
pair<float, bool> get_value_and_terminated(State &state, string &_action)
{
	(void)_action;
	int games_over = 0;
	for (int game = 0; game < 3; game++)
		games_over += (state.games_gpu[game] == "GAME_OVER");
	if (games_over < 3)	return {0, false};
	//
	/* here calculate the score and get my ranking position */
	vector<pair<int, int>> players_scores(3);
	for (int player = 0; player < 3; player++)
	{
		players_scores[player].second = player;
		players_scores[player].first = state.players_final_scores[player];
	}
	sort(players_scores.rbegin(), players_scores.rend());
	for (int player = 0; player < 3; player++)
	{
		players_scores[player].first = players_scores[player].second;
		players_scores[player].second = player;
	}
	sort(players_scores.begin(), players_scores.end());
	vector<float> values = {1.0f, 0.5f, 0};
	return {values[players_scores[player_idx].second], true};
}

string get_random_action()
{
	string actions = "UDLR";
	string rand_action;
	for (int i = 0; i < 3; i++)
		rand_action += actions[rand() % 4];
	return rand_action;
}

void update_archery(State &state, const string &action)
{
	string &winds = state.games_gpu[1];
	vector<int> &regs = state.games_regs[1];

	if (winds == "GAME_OVER") return ;

	for (int player = 0; player < 3; player++)
	{
		char act = action[player];
		int offcet = winds[0] - '0';
		int dx = 0;
		int dy = 0;
		if (act == 'D') dy = offcet;
		else if (act == 'L') dx = -offcet;
		else if (act == 'R') dx = offcet;
		else if (act == 'U') dy = -offcet;
		int &posx = regs[player*2];
		int &posy = regs[player*2 + 1];
		posx += dx;
		posy += dy;
		if (posx > 20) posx = 20;
		if (posx < -20) posx = -20;
		if (posy > 20) posy = 20;
		if (posy < -20) posy = -20;
	}

	if (winds.size() > 1) winds = winds.substr(1, winds.size() - 1);
	else 
	{
		winds = "GAME_OVER";
		map<double, vector<int>> result;
		for (int player = 0; player < 3; player++)
		{
			int posx = regs[player*2], posy = regs[player*2+1];
			double dis = posx*posx + posy*posy;
			result[dis].push_back(player);
		}
		int rank = 0;
		for (auto i = result.begin(); i != result.end(); i++)
		{
			for (auto p : i->second)
			{
				if (rank == 0)
					state.players_medals[p][1].gold += 1;
				else if (rank == 1)
					state.players_medals[p][1].silver += 1;
				else
					state.players_medals[p][1].bronze += 1;
			}
			rank++;
		}
	}
}

void update_hurdle_race(State &state, const string &action)
{
	string &gpu = state.games_gpu[0];
	vector<int> &regs = state.games_regs[0];
	//
	if (gpu == "GAME_OVER") return;
	//
	bool finished = false;
	for (int player = 0; player < 3; player++)
	{
		pair<int, bool> act;
		if (action[player] == 'R') act = {3, false};
		else if (action[player] == 'U') act = {2, true};
		else if (action[player] == 'D') act = {2, false};
		else if (action[player] == 'L') act = {1, false};
		int &pos = regs[player];
		int &stunned = regs[player + 3];
		if (stunned)
		{
			stunned -= 1;
			continue;
		}
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
	if (finished)
	{
		gpu = "GAME_OVER";
		map<double, vector<int>> result;
		for (int player = 0; player < 3; player++)
		{
			result[-1 * regs[player]].push_back(player);
		}
		int rank = 0;
		for (auto i = result.begin(); i != result.end(); i++)
		{
			for (auto p : i->second)
			{
				if (rank == 0)
					state.players_medals[p][0].gold += 1;
				else if (rank == 1)
					state.players_medals[p][0].silver += 1;
				else
					state.players_medals[p][0].bronze += 1;
			}
			rank++;
		}
	}
}

void update_diving(State &state, const string &action)
{
	string &gpu = state.games_gpu[2];
	vector<int> &regs = state.games_regs[2];
	if (gpu == "GAME_OVER") return ;
	for (int player = 0; player < 3; player++)
	{
		int &point = regs[player];
		int &combo = regs[player + 3];
		if (action[player] == gpu[0])
		{
			combo += 1;
			point += combo;
		}
		else combo = 0;
	}
	//
	if (gpu.size() > 1) gpu = gpu.substr(1, gpu.size() - 1);
	else
	{
		gpu = "GAME_OVER";
		map<double, vector<int>> result;
		for (int player = 0; player < 3; player++)
		{
			result[regs[player]].push_back(player);
		}
		int rank = 0;
		for (auto i = result.begin(); i != result.end(); i++)
		{
			for (auto p : i->second)
			{
				if (rank == 0)
					state.players_medals[p][2].gold += 1;
				else if (rank == 1)
					state.players_medals[p][2].silver += 1;
				else
					state.players_medals[p][2].bronze += 1;
			}
			rank++;
		}
	}
}

void update_final_score(State &state)
{
	for (int p = 0; p < 3; p++)
	{
		state.players_final_scores[p] = 1;
		for (int g = 0; g < 3; g++)
		{
			state.players_final_scores[p] *= (state.players_medals[p][g].gold * 3
				+ state.players_medals[p][g].silver);
		}
	}
}

State get_next_state(const State &state, const string &action)
{
	State nextState = state;
	update_hurdle_race(nextState, action);
	update_archery(nextState, action);
	update_diving(nextState, action);
	update_final_score(nextState);
	return (nextState);
}
/*node-for-mcts-class*/
struct Node
{
	State state;
	string action_taken;
	vector<string> expandable_actions;
	Node *parent;
	vector<Node*> childrens;
	float visit_count;
	float value_sum;
	//
	Node(const State &_state,
		Node *_parent = nullptr,
		const string &_action_taken = "")
	{
		state = _state;
		parent = _parent;
		action_taken = _action_taken;
		visit_count = 0;
		value_sum = 0;
		expandable_actions = mcts_actions_list;
	}

	bool is_fully_expanded()
	{
		return (expandable_actions.empty() && childrens.size() > 0);
	}

	float get_ucb(Node *child)
	{
		float qv = (child->value_sum / child->visit_count);
		return qv + (1.41f * (sqrt(visit_count) / (child->visit_count * 1.0f)));
	}

	Node *select()
	{
		map<char, pair<Node*, float>> ucb;
		for (auto i : childrens)
		{
			float curUcb = this->get_ucb(i);
			//
			if (ucb.count(i->action_taken[player_idx]))
			{
				if (ucb[i->action_taken[player_idx]].second > curUcb)
				{
					ucb[i->action_taken[player_idx]].first = i;
					ucb[i->action_taken[player_idx]].second = curUcb;
				}
			}
			else
			{
				ucb[i->action_taken[player_idx]].first = i;
				ucb[i->action_taken[player_idx]].second = curUcb;
			}
		}
		//
		Node *ans = nullptr;
		float ansUcb = -INF;
		for (auto [i, j] : ucb)
			if (j.second > ansUcb)
			{
				ans = j.first;
				ansUcb = j.second;
			}
		//
		return (ans);
	}

	Node *expand()
	{
		int r = rand() % expandable_actions.size();
		string action = expandable_actions[r];
		swap(expandable_actions[r], expandable_actions[expandable_actions.size() - 1]);
		expandable_actions.pop_back();
		//
		childrens.push_back(new Node(get_next_state(state, action), this, action));
		return (childrens.back());
	}

	float simulate()
	{
		State curState = state;
		while (true)
		{
			auto x = get_value_and_terminated(curState, action_taken);
			if (x.second) return (x.first);
			curState = get_next_state(curState, get_random_action());
		}
		return (0.0f);
	}

	void backpropagate(float value)
	{
		value_sum += value;
		visit_count += 1;
		if (parent) parent->backpropagate(value);
	}
};
/*mcts-class*/
class MCTS
{
public:

	void debug(Node *it)
	{
		cerr << "action-taken: " << it->action_taken << ", ";
		cerr << "parent-adress: " << it->parent << endl;
		cerr << "Value=" << it->value_sum << ", ";
		cerr << "Visits=" << it->visit_count << endl;
		cerr << it->state << endl;

		for (auto i : it->childrens) debug(i);
	}

	string search(int number_of_iterations, State &state)
	{
		Node *root = new Node(state);

		for (int i = 0; i < number_of_iterations; i++)
		{
			Node *it = root;

			// debug(it);

			while (it->is_fully_expanded())
				it = it->select();

			pair<float, bool> x = get_value_and_terminated(it->state, it->action_taken);
			float value = x.first;
			bool is_terminated = x.second;

			if (!is_terminated)
			{
				it = it->expand();
				value = it->simulate();
			}

			it->backpropagate(value);
		}

		// map<char, vector<float>> prob;

		// float visit_sum = 0;
		// for (auto i : root->childrens)
		// {
		// 	visit_sum += i->visit_count;
		// 	prob[i->action_taken[player_idx]].push_back(i->visit_count);
		// }

		Node *it = root->select();
		char ansAction = it->action_taken[player_idx];
		// float ansProb = -INF;

		// for (auto i = prob.begin(); i != prob.end(); i++)
		// {
		// 	float curProb = INF;
		// 	for (auto &j : i->second)
		// 	{
		// 		j /= visit_sum;
		// 		curProb = min(curProb, j);
		// 	}
		// 	if (curProb > ansProb)
		// 	{
		// 		ansProb = curProb;
		// 		ansAction = i->first;
		// 	}
		// }

		if (ansAction == 'U') return "UP";
		else if (ansAction == 'D') return "DOWN";
		else if (ansAction == 'L') return "LEFT";
		else if (ansAction == 'R') return "RIGHT";

		return (string(1, ansAction));
	}
};
/* solve function*/
void solve(State &state)
{
	MCTS mcts;

	string mcts_bestAction = mcts.search(300, state);

	cout << mcts_bestAction << endl;
}

/*main-function*/
void generate_mcts_actions_list(const string &acts = "UDRL", string cur = "")
{
	if (cur.size() == 3)
	{
		mcts_actions_list.push_back(cur);
		return ;
	}
	for (char c : acts)
		generate_mcts_actions_list(acts, cur + c);
}

int main()
{
	cin >> player_idx; cin.ignore();
	int nb_games;
	cin >> nb_games; cin.ignore();
	generate_mcts_actions_list();

	// game loop
	while (true)
	{
		State state;

		for (int i = 0; i < 3; i++)
		{
			string score_info;
			getline(cin, score_info);

			stringstream S(score_info);
			S >> state.players_final_scores[i];
			for (int g = 0; g < 4; g++)
			{
				int gold, silve, bronze;
				S >> gold >> silve >> bronze;
				if (g==0 || g == 1)
				{
					state.players_medals[i][g].gold = gold;
					state.players_medals[i][g].silver = silve;
					state.players_medals[i][g].bronze = bronze;
				}
				else if (g == 3)
				{
					state.players_medals[i][2].gold = gold;
					state.players_medals[i][2].silver = silve;
					state.players_medals[i][2].bronze = bronze;
				}
			}
		}

		for (int g = 0; g < nb_games; g++) 
		{
			string gpu;
			cin >> gpu;
			vector<int> reg(7);
			for (int j = 0; j < 7; j++) cin >> reg[j];


			if (g==0 || g == 1)
			{
				state.games_gpu[g] = gpu;
				state.games_regs[g] = reg;
			}
			else if (g == 3)
			{
				state.games_gpu[2] = gpu;
				state.games_regs[2] = reg;
			}

			cin.ignore();
		}

		solve(state);
	}
}
