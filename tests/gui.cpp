#include <yttrium/file.h>
#include <yttrium/gui.h>
#include <yttrium/log.h>
#include <yttrium/script/manager.h>
#include <yttrium/string.h>
#include <yttrium/window.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_gui)
{
	DECLARE_MEMORY_MANAGER;

	File file(File::Temporary);

	{
		LogManager log_manager(DefaultAllocator); // For error reporting.

		ScriptManager script_manager(DefaultAllocator); // For scripted actions.

		WindowCallbacks window_callbacks;
		const auto window = Window::create(window_callbacks);
		BOOST_REQUIRE(window);
		BOOST_REQUIRE(window->gui().load("tests/gui/gui.ion"));
		window->gui().dump(file.name());
	}

	String expected;
	String actual;

	BOOST_REQUIRE(File("tests/gui/dump.ion").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);
}
