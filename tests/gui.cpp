#include <yttrium/file.h>
#include <yttrium/gui.h>
#include <yttrium/log.h>
#include <yttrium/pointer.h>
#include <yttrium/script/context.h>
#include <yttrium/string.h>
#include <yttrium/window.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_gui)
{
	DECLARE_MEMORY_MANAGER;

	File file(File::Temporary);

	{
		ScriptContext script_context;
		WindowCallbacks window_callbacks;
		const auto& window = Window::create(script_context, window_callbacks);
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
