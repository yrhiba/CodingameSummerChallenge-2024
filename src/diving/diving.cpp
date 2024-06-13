#include "header.hpp"
/*start*/

string getDivingBestAction(MiniGame &diving)
{
	if (diving.gpu == "GAME_OVER") return (diving.gpu);

	for (string action : actions)
		if (action[0] == diving.gpu[0]) return (action);

	return (""); // impossible to happen.
}

/*end*/
