#include <yttrium/log.h>
#include <yttrium/memory_manager.h>
#include <yttrium/proxy_allocator.h>
#include <yttrium/script/manager.h>

#include "game.h"

int main(int, char **)
{
	MemoryManager memory_manager;
	LogManager log_manager("tetrium.log");
	ScriptManager script_manager;

	ProxyAllocator game_allocator("game");
	Game game(&game_allocator);

	if (game.setup())
		game.run();
	return 0;
}
