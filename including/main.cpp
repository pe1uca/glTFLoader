#include "Game.h"
//#include <vld.h>

int main()
{
	Game game;

	if (!game.init())
	{
		game.release();
		return game.GetExitValue();
	}

	game.run();

	game.release();

	return game.GetExitValue();
}
