#pragma once
#include "header.hpp"

/*start*/

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
