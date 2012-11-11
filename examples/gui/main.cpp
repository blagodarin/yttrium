#include <Yttrium/yttrium.h>

using namespace Yttrium;

class GuiExample: public Terminal::Callbacks
{
public:

	GuiExample()
		: _log_manager("gui.log")
		, _terminal(this)
	{
		_script_manager.root_context().define("quit", std::bind(&GuiExample::quit, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}

	void run()
	{
		if (!_terminal.open())
		{
			return;
		}

		_terminal.resize(800, 600);
		_terminal.show();

		Renderer renderer = _terminal.create_renderer(Renderer::OpenGl);

		if (!renderer)
		{
			return;
		}

		_gui = Gui::Manager::create(renderer);

		if (_gui.is_null() || !_gui->load("examples/gui/gui.ion"))
		{
			return;
		}

		RateCounter fps;

		fps.start();

		for (; ; )
		{
			if (!_terminal.process_events())
			{
				break;
			}

			renderer.begin_frame();
			_gui->set_cursor(_terminal.cursor());
			_gui->render();
			renderer.end_frame();

			ScriptManager::instance()->root_context().set("fps", fps.rate());
			fps.tick();
		}
	}

private:

	void quit(const StaticString &, String *, const ScriptArgs &) noexcept
	{
		_terminal.close();
	}

private: // Terminal::Callbacks

	virtual void on_cursor_movement(Terminal *, const Dim2 &) noexcept
	{
	}

	virtual bool on_key_event(Terminal *terminal, Key key, KeyState state) noexcept
	{
		return _gui->process_key(terminal, key, state);
	}

private:

	MemoryManager   _memory_manager;
	LogManager      _log_manager;
	ScriptManager   _script_manager;
	Terminal        _terminal;
	Gui::ManagerPtr _gui;
};

int main()
{
	GuiExample example;

	example.run();
	return 0;
}
