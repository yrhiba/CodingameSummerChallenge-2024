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
using namespace std;

vector<string> files =
{
	/* includes-file */
	"incs/header.hpp",
	"incs/state.hpp",

	/* best moves */
	"src/archery_bestMove.cpp",

	/* */
	"src/game.cpp",
	"src/scoring.cpp",
	"src/search.cpp",

	/*main-file*/
	"src/main.cpp",
};

int main()
{
	ofstream out("./prod/code.cpp");

	out << "/* yrhiba.github.io (c). 2022-2024 */\n" << endl;

	for (auto &file : files)
	{
		ifstream	fin(file);
		string		content;
		while (content != "/*start*/")
			getline(fin, content);
		out << "// ########################" << endl;
		out << "// file-Name: " << file << endl;
		out << "// ########################" << endl;
		while (getline(fin, content) && content != "/*end*/")
		{
			out << content << endl;
		}
	}
	out.close();
	return (0);
}
