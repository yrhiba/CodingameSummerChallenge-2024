#include "header.hpp"
#include "state.hpp"
pair<float, bool> get_value_and_terminated(State &state);
/*start*/

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
		if (duration > 45)
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
