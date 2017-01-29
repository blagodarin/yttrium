#include <yttrium/audio/manager.h>
#include <yttrium/audio/player.h>
#include <yttrium/gui/gui.h>
#include <yttrium/memory/unique_ptr.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/script/context.h>
#include <yttrium/storage/storage.h>
#include <yttrium/string.h>
#include <yttrium/window.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_gui)
{
	Storage storage(Storage::UseFileSystem::Before);
	ResourceLoader resource_loader(storage);
	Window window("GUI test");
	ScriptContext script_context;
	Gui gui(resource_loader, script_context, "tests/gui/gui.ion");
}
