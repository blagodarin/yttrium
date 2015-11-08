#include "game.h"

#include <yttrium/log_manager.h>
#include <yttrium/memory/manager.h>

int main(int, char**)
{
	MemoryManager memory_manager;

	ProxyAllocator log_allocator("log"_s, *memory_manager.default_allocator());
	const auto& log_manager = LogManager::create("rts.log", log_allocator);

	Game game(*memory_manager.default_allocator());
	game.run();
	return 0;
}
