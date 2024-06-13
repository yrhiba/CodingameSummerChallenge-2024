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
	int player_idx;
	int nb_games;
	vector<string> playersScore = vector<string>(3);
	vector<MiniGame> minGames = vector<MiniGame>(4);

}	game;


/*
give in sorted order the list
of best action with there next
distance to center (0, 0);
for archery mini game.
*/
vector<pair<int, string>> getArcheryBestActionOrder(MiniGame &archey);

