#include "game.h"

#include <yttrium/i18n/localization.h>
#include <yttrium/log.h>
#include <yttrium/package.h>

int main()
{
	Log::set_file("tetrium.log");

	NamedAllocator i18n_allocator("i18n");
	const auto& localization = Localization::create("examples/tetrium/i18n/en.ion", i18n_allocator);

	NamedAllocator package_allocator("package");
	const auto& package_manager = PackageManager::create(PackageManager::Order::PackedOnly, package_allocator);
	if (!package_manager->mount("tetrium.ypq"))
		return 1;

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
