#pragma once
/*start*/
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
#include <fstream>
#include <cassert>
#include <random>
#include <sstream>
using namespace std;

// macros.
# define all(x) begin(x), end(x)
# define rall(x) rbegin(x), rend(x)
# define ign cin.ignore();

// for archery directions.
const string actions[8] {"DOWN","RIGHT","UP","LEFT"};
const int dr[4] {1, 0, -1, 0};
const int dc[4] {0, 1, 0, -1};
// hurdle race movesBy, Jump
const pair<int, int> mov[4] {{2, 0},{3, 0},{2, 1},{1, 0}};

// miniGame struct
struct MiniGame
{
	string gpu;
	vector<int> regs = vector<int>(7);
};

// global Game structer
struct Game
{
	int running = true;
	int turn = -1;
	int player_idx;
	int nb_games;
	vector<string> playersScore = vector<string>(3);
	vector<MiniGame> minGames = vector<MiniGame>(4);

}	game;


/*
return a sorted order list
of best action with there next
distance to center (0, 0);
for archery mini game.
*/
vector<pair<int, string>> getArcheryBestActionOrder(MiniGame &archey);


/*
return best action to increment the combo
and get the maximum score.
*/
string getDivingBestAction(MiniGame &diving);


/*
return ordered actions for hurdle race sorted
by how far gonna make my player move.
*/
vector<pair<int, string>> getHurdleActionOrder(MiniGame &hurdle);
