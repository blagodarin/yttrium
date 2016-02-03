#include "game.h"

#include <yttrium/log_manager.h>
#include <yttrium/static_string.h>

int main(int, char**)
{
	const auto& log_manager = LogManager::create("3d.log");
	Game game;
	game.run();
	return 0;
}
