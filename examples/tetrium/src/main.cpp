#include "game.h"

#include <yttrium/i18n/localization.h>
#include <yttrium/log_manager.h>
#include <yttrium/memory_manager.h>

int main(int, char**)
{
	MemoryManager memory_manager;

	ProxyAllocator log_allocator("log"_s, *memory_manager.default_allocator());
	const auto& log_manager = LogManager::create("tetrium.log", log_allocator);

	ProxyAllocator i18n_allocator("i18n"_s, *memory_manager.default_allocator());
	const auto& localization = Localization::create("examples/tetrium/i18n/en.ion", i18n_allocator);

	Game game(*memory_manager.default_allocator());
	game.run();
	return 0;
}
