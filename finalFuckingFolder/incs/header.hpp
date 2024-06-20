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
using namespace std;

# define all(x) begin(x), end(x)
# define rall(x) rbegin(x), rend(x)

int player_idx, nb_games;

/*end*/

/*game-utils*/
State get_next_state(const State &state, char action);