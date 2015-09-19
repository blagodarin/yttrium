#include "game.h"

#include <yttrium/i18n/localization.h>
#include <yttrium/log_manager.h>
#include <yttrium/memory_manager.h>

int main(int, char**)
{
	MemoryManager memory_manager;

	const auto& log_manager = LogManager::create("tetrium.log");
	const auto& localization = Localization::create("examples/tetrium/i18n/en.ion");

	Game game;
	game.run();
	return 0;
}
