#include <Yttrium/file.h>
#include <Yttrium/gui/manager.h>
#include <Yttrium/log.h>
#include <Yttrium/memory_manager.h>
#include <Yttrium/window.h>
#include <Yttrium/string.h>

#define BOOST_TEST_MODULE gui

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_FIXTURE_TEST_SUITE(test_suite_memory_manager, MemoryManager)

BOOST_AUTO_TEST_CASE(gui_test)
{
	// NOTE: Unfortunately, GUI manager requires log in case something goes wrong.

	LogManager log_manager("test_gui.log");

	File file(File::Temporary);

	{
		Screen screen;

		BOOST_REQUIRE(screen.open()); // NOTE: Being run in the test set, it fails for unknown reason.

		Window window;

		BOOST_REQUIRE(window.open(screen, nullptr));

		Renderer renderer = window.create_renderer(Renderer::OpenGl);

		BOOST_REQUIRE(renderer);

		Gui::ManagerPtr manager = Gui::Manager::create(renderer);

		BOOST_REQUIRE(manager->load("tests/gui/gui.ion"));
		manager->dump(file.name());
	}

	String expected;
	String actual;

	BOOST_REQUIRE(File("tests/gui/dump.ion").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);
}

BOOST_AUTO_TEST_SUITE_END()
