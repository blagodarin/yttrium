#include <yttrium/gui/gui.h>
#include <yttrium/resource_loader.h>
#include <yttrium/script/context.h>
#include <yttrium/static_string.h>
#include <yttrium/storage/storage.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_gui)
{
	Storage storage(Storage::UseFileSystem::Before);
	ResourceLoader resource_loader(storage);
	ScriptContext script_context;
	Gui gui(resource_loader, script_context, "tests/gui/gui.ion");
}
