#include <yttrium/file.h>
#include <yttrium/gui.h>
#include <yttrium/log.h>
#include <yttrium/script/manager.h>
#include <yttrium/string.h>
#include <yttrium/window.h>

#include "common.h"

using namespace Yttrium;

class WindowCallbacks: public Window::Callbacks
{
public:

	void on_cursor_movement(Window&, const Dim2&) noexcept override {}
	bool on_key_event(const KeyEvent&) noexcept override { return true; }
};

class GuiCallbacks: public Gui::Callbacks
{
public:

	void on_render_canvas(Renderer&, const StaticString&, const RectF&) noexcept override {}
};

BOOST_AUTO_TEST_CASE(test_gui)
{
	DECLARE_MEMORY_MANAGER;

	File file(File::Temporary);

	{
		WindowCallbacks window_callbacks;
		WindowPtr window = Window::open(Dim2(320, 240), window_callbacks);
		BOOST_REQUIRE(!window.is_null());

		Renderer renderer = window->create_renderer();
		BOOST_REQUIRE(renderer);

		// Unfortunately, GUI requires log in case something goes wrong.
		LogManager log_manager("test_gui.log", DefaultAllocator);

		ScriptManager script_manager(DefaultAllocator); // For scripted actions.

		GuiCallbacks gui_callbacks;
		GuiPtr gui = Gui::create(renderer, gui_callbacks);

		BOOST_REQUIRE(gui->load("tests/gui/gui.ion"));
		gui->dump(file.name());
	}

	String expected;
	String actual;

	BOOST_REQUIRE(File("tests/gui/dump.ion").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);
}
