#include "game.h"

#include <yttrium/i18n/localization.h>
#include <yttrium/log.h>

int main()
{
	Log::set_file("tetrium.log");

	NamedAllocator i18n_allocator("i18n"_s);
	const auto& localization = Localization::create("examples/tetrium/i18n/en.ion", i18n_allocator);

	try
	{
		Game game;
		game.run();
		return 0;
	}
	catch (const std::runtime_error& e)
	{
		Log() << "Runtime error: " << e.what();
		return 1;
	}
}
