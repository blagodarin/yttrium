#include "game.h"

#include <yttrium/log_manager.h>
#include <yttrium/memory/manager.h>

int main(int, char**)
{
	MemoryManager memory_manager;
	const auto& log_manager = LogManager::create("3d.log");
	Game game;
	game.run();
	return 0;
}
