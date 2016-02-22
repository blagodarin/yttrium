#include "game.h"

#include <yttrium/log.h>

int main(int, char**)
{
	Log::set_file("3d.log");
	Game game;
	game.run();
	return 0;
}
