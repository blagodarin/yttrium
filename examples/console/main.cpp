#include <Yttrium/yttrium.h>

using namespace Yttrium;

int main()
{
	MemoryManager memory_manager;

	LogManager log_manager("console.log");

	Terminal terminal;

	if (terminal.open())
	{
		terminal.resize(800, 600);
		terminal.show();

		terminal.set_console_visible(true);

		Renderer renderer = terminal.create_renderer(Renderer::OpenGl);

		if (renderer)
		{
			RendererBuiltin renderer_builtin = renderer.renderer_builtin();

			for (; ; )
			{
				if (!terminal.process_events())
				{
					break;
				}

				renderer.begin_frame();
				terminal.draw_console(&renderer_builtin);
				renderer.end_frame();
			}
		}
	}

	return 0;
}
