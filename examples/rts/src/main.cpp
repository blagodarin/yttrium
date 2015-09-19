#include "game.h"

#include <yttrium/log_manager.h>
#include <yttrium/memory_manager.h>

int main(int, char**)
{
	MemoryManager memory_manager;

	const auto& log_manager = LogManager::create("rts.log");

	Game game;
	game.run();
	return 0;
}
