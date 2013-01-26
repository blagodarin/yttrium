#include <Yttrium/log.h>
#include <Yttrium/memory_manager.h>
#include <Yttrium/proxy_allocator.h>
#include <Yttrium/script/manager.h>

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
