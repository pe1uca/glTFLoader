#include "Game.h"


int main()
{
	Game* game = new Game();

	if (!game->init())
	{
		return game->GetExitValue();
	}

	game->run();

	game->release();

	return game->GetExitValue();
}
