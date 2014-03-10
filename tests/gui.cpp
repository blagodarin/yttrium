#include <yttrium/file.h>
#include <yttrium/gui.h>
#include <yttrium/log.h>
#include <yttrium/string.h>
#include <yttrium/window.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(gui_test)
{
	DECLARE_MEMORY_MANAGER;

	// Unfortunately, GUI manager requires log in case something goes wrong.
	LogManager log_manager("test_gui.log", DefaultAllocator);

	File file(File::Temporary);

	{
		WindowPtr window = Window::open(Dim2(320, 240), nullptr);
		BOOST_REQUIRE(!window.is_null());

		Renderer renderer = window->create_renderer();
		BOOST_REQUIRE(renderer);

		GuiPtr gui = Gui::create(renderer);

		BOOST_REQUIRE(gui->load("tests/gui/gui.ion"));
		gui->dump(file.name());
	}

	String expected;
	String actual;

	BOOST_REQUIRE(File("tests/gui/dump.ion").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);
}
