#include <yttrium/gui.h>
#include <yttrium/io/storage.h>
#include <yttrium/memory/unique_ptr.h>
#include <yttrium/script/context.h>
#include <yttrium/string.h>
#include <yttrium/window.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_gui)
{
	Storage storage(Storage::UseFileSystem::Before);
	WindowCallbacks window_callbacks;
	Window window("GUI test", window_callbacks);
	ScriptContext script_context;
	Gui gui(storage, window.renderer(), script_context);
	BOOST_REQUIRE(gui.load("tests/gui/gui.ion"));
}
