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

	if (!_window->gui().load("examples/rts/gui.ion"))
		return;

	create_cube();

	_window->show();
	_window->run();
}

void Game::on_key_event(const KeyEvent& event)
{
	if (!event.pressed)
		return;

	switch (event.key)
	{
	case Key::Up: _pitch += 5; break;
	case Key::Down: _pitch -= 5; break;
	case Key::Left: _yaw -= 5; break;
	case Key::Right: _yaw += 5; break;

	case Key::Escape:
		_window->close();
		break;

	case Key::A: _position.x -= 1; break;
	case Key::D: _position.x += 1; break;
	case Key::E: _roll += 5; break;
	case Key::F: _position.y -= 1; break;
	case Key::Q: _roll -= 5; break;
	case Key::R: _position.y += 1; break;
	case Key::S: _position.z += 1; break;
	case Key::W: _position.z -= 1; break;

	case Key::Grave:
		_window->set_console_visible(!_window->is_console_visible());
		break;

	case Key::F1:
		_window->set_debug_text_visible(!_window->is_debug_text_visible());
		break;

	case Key::F10: // KDE grabs Key::Print. =(
		_window->take_screenshot(String::format(DateTime::now(), "%YY-%MM-%DD_%hh-%mm-%ss.png", &_allocator));
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
		Matrix4::perspective(static_cast<float>(window_size.width) / window_size.height,
			90,       // Vertical FOV angle in degrees.
			1, 100)); // Near/far plane distance.

	PushTransformation camera(renderer, Matrix4::camera(_position, _pitch, _yaw, _roll));

	// Center.
	draw_cube(renderer, Vector4(0, 0, 0));

	// X direction -- one cube.
	draw_cube(renderer, Vector4(2, 0, 0));

	// Y direction -- two cubes.
	draw_cube(renderer, Vector4(0, 2, 0));
	draw_cube(renderer, Vector4(0, 4, 0));

	// Z direction -- three cubes.
	draw_cube(renderer, Vector4(0, 0, 2));
	draw_cube(renderer, Vector4(0, 0, 4));
	draw_cube(renderer, Vector4(0, 0, 6));

	const auto angle = Timer::clock() / 5 % 360;

	for (int i = 0; i < 10; ++i)
	{
		const auto z = -4 - 2 * i;
		{
			PushTransformation transformation(renderer, Matrix4::rotation(angle, Vector4(0, 1, 0)));
			draw_cube(renderer, Vector4( 2, -2,  z));
			draw_cube(renderer, Vector4( 2, -2, -z));
		}
		{
			PushTransformation transformation(renderer, Matrix4::rotation(angle, Vector4(1, 0, 0)));
			draw_cube(renderer, Vector4(-2, -2,  z));
			draw_cube(renderer, Vector4(-2, -2, -z));
		}
		{
			PushTransformation transformation(renderer, Matrix4::rotation(angle, Vector4(-1, 0, 0)));
			draw_cube(renderer, Vector4( 2,  2,  z));
			draw_cube(renderer, Vector4( 2,  2, -z));
		}
		{
			PushTransformation transformation(renderer, Matrix4::rotation(angle, Vector4(0, -1, 0)));
			draw_cube(renderer, Vector4(-2,  2,  z));
			draw_cube(renderer, Vector4(-2,  2, -z));
		}
	}
}

void Game::on_update(const UpdateEvent& update)
{
	_window->debug_text().clear()
		<< "FPS: " << update.fps << "\n"
		<< "MaxFrameTime: " << update.max_frame_time << "\n"
		<< "X: " << _position.x << ", Y: " << _position.y << ", Z: " << _position.z << "\n"
		<< "Pitch: " << _pitch << ", Yaw: " << _yaw << ", Roll: " << _roll;
}

void Game::create_cube()
{
	struct Vertex3D
	{
		Vector4 position;
		Vector4 color;

		Vertex3D(const Vector4& position, const Vector4& color)
			: position(position)
			, color(color)
		{
		}
	};

	const std::array<Vertex3D, 8> vertices =
	{
		Vertex3D(Vector4(-0.5, -0.5, -0.5), Vector4(0, 0, 0)),
		Vertex3D(Vector4(+0.5, -0.5, -0.5), Vector4(0, 0, 1)),
		Vertex3D(Vector4(-0.5, +0.5, -0.5), Vector4(0, 1, 0)),
		Vertex3D(Vector4(+0.5, +0.5, -0.5), Vector4(0, 1, 1)),
		Vertex3D(Vector4(-0.5, -0.5, +0.5), Vector4(1, 0, 0)),
		Vertex3D(Vector4(+0.5, -0.5, +0.5), Vector4(1, 0, 1)),
		Vertex3D(Vector4(-0.5, +0.5, +0.5), Vector4(1, 1, 0)),
		Vertex3D(Vector4(+0.5, +0.5, +0.5), Vector4(1, 1, 1)),
	};

	const std::array<uint16_t, 36> indices =
	{
		// Bottom.
		0, 2, 1,
		1, 2, 3,

		// Front.
		0, 1, 4,
		4, 1, 5,

		// Top.
		4, 5, 6,
		6, 5, 7,

		// Back.
		2, 6, 3,
		3, 6, 7,

		// Right.
		5, 1, 7,
		7, 1, 3,

		// Left.
		2, 0, 6,
		6, 0, 4,
	};

	_cube_vertices = _window->renderer().create_vertex_buffer(VertexBuffer::Rgba4F, vertices.size(), &vertices[0]);
	_cube_indices = _window->renderer().create_index_buffer(IndexBuffer::Format::U16, indices.size(), &indices[0]);
}

void Game::draw_cube(Renderer& renderer, const Vector4& center)
{
	PushTransformation transformation(renderer, Matrix4::translation(center));
	renderer.draw_triangles(*_cube_vertices, *_cube_indices);
}
