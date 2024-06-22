#include "header.hpp"
#include "state.hpp"
#include <sstream>
#include <iostream>
string archery_getBestMove(string &gpu, int x, int y);
void update_hurdle_data(string &gpu, vector<int> &regs);
void update_archery_data(string &gpu, vector<int> &regs);
void update_diving_data(string &gpu, vector<int> &regs);
/*start*/

void solve(State &state)
{
	string action;

	if (scoring.archery_score_weight == 1.0f)
	{
		action = archery_getBestMove(
				state.archery_gpu,
				state.archery_x,
				state.archery_y
			);
	}
	else
	{
		action = search(10000, state);
	}

	cout << action << endl;
}

int main()
{
	cin.tie(0)->sync_with_stdio(0);
	srand(time(0));

	cin >> player_idx >> nb_games;
	cin.ignore();

	while (true)
	{
		stringstream tunrInput;
		State state;
		vector<string> scores(3);
		for (int i = 0; i < 3; i++)
		{
			getline(cin, scores[i]);
		}

		#if PRINT_INPUT
		tunrInput << "---start-turn-input---------" << endl;
		for (int i = 0; i < 3; i++)
			tunrInput << scores[i] << "\n";
		#endif

		for (int i = 0; i < 4; i++)
		{
			string gpu;
			cin >> gpu;
			vector<int> regs(7);
			for (int r=0;r<7;r++)cin>>regs[r];
			cin.ignore();

			#if PRINT_INPUT
			tunrInput << gpu << endl;
			for (int r=0;r<7; r++)
				tunrInput << regs[r] << " \n"[r+1==7];
			#endif

			//
			if (i == 0)
			{
				state.hurdle_gpu = gpu;
				state.hurdle_pos = regs[player_idx];
				state.hurdle_stunned = regs[player_idx+3];
				state.hurdle_turn = 0;
				update_hurdle_data(gpu, regs);
			}
			else if (i == 1)
			{
				state.archery_gpu = gpu;
				state.archery_x = regs[player_idx*2];
				state.archery_y = regs[player_idx*2+1];
				update_archery_data(gpu, regs);
			}
			else if (i == 3)
			{
				state.diving_gpu = gpu;
				state.diving_point = regs[player_idx];
				state.diving_combo = regs[player_idx+3];
				update_diving_data(gpu, regs);
			}
		}

		#if PRINT_INPUT
		tunrInput << "-----end-turn-input---------";
		#endif

		cerr << "Turn-round: " << turn << endl;

		#if PRINT_INPUT
		cerr << tunrInput.str() << endl;
		#endif

		reverse(rall(state.archery_gpu)); // reversing gpu for fast archery
		reverse(rall(state.diving_gpu)); // reversing gpu for fast diving
		scoring.evaluateAndSetWeights();
		solve(state);
		turn += 1;
	}
}
