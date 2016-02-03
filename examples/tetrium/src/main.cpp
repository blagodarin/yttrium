#include "game.h"

#include <yttrium/i18n/localization.h>
#include <yttrium/log_manager.h>
#include <yttrium/static_string.h>

int main(int, char**)
{
	ProxyAllocator log_allocator("log"_s);
	const auto& log_manager = LogManager::create("tetrium.log", log_allocator);

	ProxyAllocator i18n_allocator("i18n"_s);
	const auto& localization = Localization::create("examples/tetrium/i18n/en.ion", i18n_allocator);

	Game game;
	game.run();
	return 0;
}
