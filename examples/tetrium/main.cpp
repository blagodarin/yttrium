#include <yttrium/log.h>
#include <yttrium/memory_manager.h>
#include <yttrium/proxy_allocator.h>
#include <yttrium/script/manager.h>

#include "game.h"

int main(int, char **)
{
	MemoryManager memory_manager;

	ProxyAllocator log_manager_allocator("log");
	LogManager log_manager("tetrium.log", &log_manager_allocator);

	ProxyAllocator script_manager_allocator("script");
	ScriptManager script_manager(&script_manager_allocator);

	ProxyAllocator game_allocator("game");
	Game game(&game_allocator);

	if (game.setup())
		game.run();
	return 0;
}
