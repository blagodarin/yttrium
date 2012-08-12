#include <Yttrium/yttrium.h>

using namespace Yttrium;

class ConsoleExample
{
public:

	ConsoleExample()
		: _log_manager("console.log")
	{
		_script_manager.root_context().define("quit", std::bind(&ConsoleExample::quit, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}

	void run()
	{
		if (_terminal.open())
		{
			_terminal.resize(800, 600);
			_terminal.show();

			_terminal.set_console_visible(true);

			Renderer renderer = _terminal.create_renderer(Renderer::OpenGl);

			if (renderer)
			{
				RendererBuiltin renderer_builtin = renderer.renderer_builtin();

				for (; ; )
				{
					if (!_terminal.process_events())
					{
						break;
					}

					renderer.begin_frame();
					_terminal.draw_console(&renderer_builtin);
					renderer.end_frame();
				}
			}
		}
	}

private:

	void quit(const StaticString &, String *, const ScriptArgs &) noexcept
	{
		_terminal.close();
	}

private:

	MemoryManager _memory_manager;
	LogManager    _log_manager;
	ScriptManager _script_manager;
	Terminal      _terminal;
};

int main()
{
	ConsoleExample example;

	example.run();
	return 0;
}
