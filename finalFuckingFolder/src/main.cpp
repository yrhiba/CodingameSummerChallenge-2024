#include "header.hpp"
#include "state.hpp"

/*start*/

void solve(State &state)
{
	cerr << state << endl;

	State nx = get_next_state(state, 'U');

	cerr << nx << endl;
}

int main()
{
	srand(time(0));
	cin >> player_idx >> nb_games;
	cin.ignore();
	while (true)
	{
		State state;
		for (int i = 0; i < 3; i++)
		{
			string score;
			getline(cin, score);
		}
		for (int i = 0; i < 4; i++)
		{
			string gpu;
			cin >> gpu;
			int regs[7];
			for (int r=0;r<7;r++)cin>>regs[r];
			cin.ignore();
			//
			if (i == 0)
			{
				state.hurdle_gpu = gpu;
				state.hurdle_pos = regs[player_idx];
				state.hurdle_stunned = regs[player_idx+3];
				state.hurdle_turn = 0;
			}
			else if (i == 1)
			{
				state.archery_gpu = gpu;
				state.archery_x = regs[player_idx*2];
				state.archery_y = regs[player_idx*2+1];
			}
			else if (i == 3)
			{
				state.diving_gpu = gpu;
				state.diving_point = regs[player_idx];
				state.diving_combo = regs[player_idx+3];
			}
		}
		reverse(rall(state.archery_gpu));
		reverse(rall(state.diving_gpu));
		solve(state);
		break;
	}
}
