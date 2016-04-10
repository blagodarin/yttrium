#include "game.h"

#include <yttrium/log.h>

int main()
{
	Log::set_file("3d.log");
	Game game;
	game.run();
}
