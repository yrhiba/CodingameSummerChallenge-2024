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

	if (!_data.hurdle_game_over && (_data.hurdle_upper_tuns_needed - _data.hurdle_lower_turns_needed) != 0)
	{
		hurlde_score = ((_data.hurdle_upper_tuns_needed - state.hurdle_turn) * 1.0f)
				/ ((_data.hurdle_upper_tuns_needed - _data.hurdle_lower_turns_needed) * 1.0f);

		hurlde_score = max((double)0.0f, hurlde_score);
		hurlde_score = min((double)1.0f, hurlde_score);
	}
	else hurlde_score = 0.0f;

	double archery_distance = sqrt(state.archery_x * state.archery_x + 
		state.archery_y * state.archery_y);

	double archery_score;

	if (!_data.archery_game_over
		&& (_data.archery_upper_distance_needed - _data.archery_lower_distance_needed) != 0)
	{
		// archery_score = 1.0f - (sqrt(archery_distance * 1.0f)
		// 		/ sqrt(20.0f * 20.0f + 20.0f * 20.0f));

		archery_score = ((_data.archery_upper_distance_needed - archery_distance) * 1.0f) / 
			((_data.archery_upper_distance_needed - _data.archery_lower_distance_needed) * 1.0f);

		archery_score = max((double)0.0f, archery_score);
		archery_score = min((double)1.0f, archery_score);
	}
	else archery_score = 0.0f;

	double diving_score;

	if (!_data.diving_game_over && (_data.diving_upper_score - _data.diving_lowest_score) != 0)
	{
		diving_score = 1.0f - (((_data.diving_upper_score - state.diving_point) * 1.0f)
			/ ((_data.diving_upper_score - _data.diving_lowest_score) * 1.0f));

		diving_score = max((double)0.0f, diving_score);
		diving_score = min((double)1.0f, diving_score);
	}
	else diving_score = 0.0f;

	double resultStateScore = scoring.finalScore(hurlde_score, archery_score, diving_score);

	return {resultStateScore, true};
}
