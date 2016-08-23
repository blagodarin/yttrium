#include <yttrium/file.h>
#include <yttrium/gui.h>
#include <yttrium/memory/unique_ptr.h>
#include <yttrium/script/context.h>
#include <yttrium/string.h>
#include <yttrium/window.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_gui)
{
	ScriptContext script_context;
	WindowCallbacks window_callbacks;
	const auto& window = Window::create(script_context, window_callbacks);
	BOOST_REQUIRE(window);
	BOOST_REQUIRE(window->gui().load("tests/gui/gui.ion"));
}
