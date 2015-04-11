#include "game.h"

#include <yttrium/gui.h>
#include <yttrium/matrix.h>
#include <yttrium/renderer.h>
#include <yttrium/time.h>

Game::Game()
	: _log_manager("rts.log")
	, _allocator("game")
{
}

void Game::run()
{
	_window = Window::create(*this, &_allocator);
	if (!_window)
		return;

	_texture_cache = _window->renderer().create_texture_cache();

	_window->set_name("Yttrium RTS example");
	_window->set_size(Size(1024, 768));
	_window->show();

	if (!_window->gui().load("examples/rts/gui.ion"))
		return;

	_window->run();
}

void Game::on_key_event(const KeyEvent& event)
{
	switch (event.key)
	{
	case Key::Escape:
		_window->close();
		break;

	case Key::Grave:
		_window->set_console_visible(!_window->is_console_visible());
		break;

	case Key::F1:
		_window->set_debug_text_visible(!_window->is_debug_text_visible());
		break;

	case Key::F10: // KDE grabs Key::Print. =(
		{
			const DateTime& now = DateTime::now();
			_window->take_screenshot(String(24, &_allocator)
				.append_dec(now.year, 4, true)
				.append('-')
				.append_dec(now.month, 2, true)
				.append('-')
				.append_dec(now.day, 2, true)
				.append('_')
				.append_dec(now.hour, 2, true)
				.append('-')
				.append_dec(now.minute, 2, true)
				.append('-')
				.append_dec(now.second, 2, true)
				.append(".png"));
		}
		break;

	default:
		break;
	}
}

void Game::on_render_canvas(Renderer& renderer, const RectF&, const StaticString& name)
{
	if (name != S("main"))
		return;

	const auto& window_size = renderer.window_size();
	PushProjection projection(renderer,
		Matrix4f::perspective(static_cast<float>(window_size.width) / window_size.height,
			60,      // Vertical FOV angle in degrees.
			1, 10)); // Near/far plane distance.

	// TODO: Draw something.
}

void Game::on_update(const UpdateEvent& update)
{
	_window->debug_text().clear()
		<< "FPS: " << update.fps << "\n"
		<< "MaxFrameTime: " << update.max_frame_time;
}
