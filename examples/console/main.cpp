#include <Yttrium/yttrium.h>

using namespace std::placeholders;
using namespace Yttrium;

class ConsoleExample
{
public:

	ConsoleExample()
		: _log_manager("console.log")
	{
		_script_manager.root_context().define("quit", std::bind(&ConsoleExample::quit, this, _1, _2, _3));
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

				TextureCachePtr texture_cache = TextureCache::create(renderer);

				Texture2D background = texture_cache->load_texture_2d("tests/image/image.tga");

				for (; ; )
				{
					if (!_terminal.process_events())
					{
						break;
					}

					renderer.begin_frame();
					renderer.set_matrix_2d(800, 600);
					renderer.set_texture(background);
					renderer.draw_rectangle(0, 0, 800, 600);
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
