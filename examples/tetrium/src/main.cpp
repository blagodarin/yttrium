#include "game.h"

#include <yttrium/i18n/localization.h>
#include <yttrium/log.h>

int main()
{
	Log::set_file("tetrium.log");

	NamedAllocator i18n_allocator("i18n"_s);
	const auto& localization = Localization::create("examples/tetrium/i18n/en.ion", i18n_allocator);

	Game game;
	game.run();
}
