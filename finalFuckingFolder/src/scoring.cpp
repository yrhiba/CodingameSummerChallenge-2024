#include "header.hpp"
#include "state.hpp"

/*start*/
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

	if ((_data.maxHurdleTurns - _data.minHurdleTurns) != 0)
	{
		hurlde_score = ((_data.maxHurdleTurns - state.hurdle_turn) * 1.0f)
				/ ((_data.maxHurdleTurns - _data.minHurdleTurns) * 1.0f);
	}
	else hurlde_score = 0.0f;

	double archery_distance = state.archery_x * state.archery_x + 
		state.archery_y * state.archery_y;

	double archery_score = 1.0f - (sqrt(archery_distance * 1.0f)
			/ sqrt(20.0f * 20.0f + 20.0f * 20.0f));

	/*
		diving notes:
			- maxDiving convert it to maxNeeded.
	*/
	double diving_score = 1.0f - (((_data.maxDivingScore - state.diving_point) * 1.0f)
		/ ((_data.maxDivingScore - _data.minDivingScore) * 1.0f));

	double resultStateScore = scoring.finalScore(hurlde_score, archery_score, diving_score);

	return {resultStateScore / 3.0f, true};
}
