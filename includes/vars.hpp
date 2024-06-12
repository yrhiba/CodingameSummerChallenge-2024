#pragma once
#include "header.hpp"
/*start*/

const int playerCount = 3;

const string gamesNames[4] = {
	"HURDLERACE", // fixed
	"HURDLERACE", // changed nexts leagues
	"HURDLERACE", // changed nexts leagues
	"HURDLERACE" // changed nexts leagues
};

int playerIdx = 0, nbGames = 4, turn = 0; // cg global data

MiniGames minGames; // arcad games data
Players players; // the 3 players data
