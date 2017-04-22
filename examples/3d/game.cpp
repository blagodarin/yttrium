#include "game.h"

#include "../utils.h"

#include <yttrium/image.h>
#include <yttrium/key.h>
#include <yttrium/math/matrix.h>
#include <yttrium/math/rect.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/string_utils.h>

#include <cmath>

Game::Game(const Storage& storage)
	: _storage{storage}
{
	_window.on_cursor_moved([this](int dx, int dy)
	{
		_rotation._yaw = wrap(_rotation._yaw - static_cast<float>(dx) / 4, -180.f, 180.f);
		_rotation._pitch = clamp(_rotation._pitch - static_cast<float>(dy) / 4, -90.f, 90.f);
	});
	_window.on_key_event([this](const KeyEvent& event){ on_key_event(event); });
	_window.on_render([this](Renderer& renderer, const PointF&){ render(renderer); });
	_window.on_screenshot([this](Image&& image){ image.save(::make_screenshot_path().c_str()); });
	_window.on_update([this](const UpdateEvent& event){ update(event); });
}

void Game::run()
{
	_window.lock_cursor(true);
	_window.show();
	_window.run();
}

void Game::on_key_event(const KeyEvent& event)
{
	switch (event.key)
	{
	case Key::Escape:
		if (event.pressed)
			_window.close();
		break;

	case Key::A:
		_move_left = event.pressed;
		break;

	case Key::D:
		_move_right = event.pressed;
		break;

	case Key::S:
		_move_backward = event.pressed;
		break;

	case Key::W:
		_move_forward = event.pressed;
		break;

	case Key::F1:
		if (event.pressed)
			_debug_text_visible = !_debug_text_visible;
		break;

	case Key::F10: // KDE grabs Key::Print. =(
		if (event.pressed)
			_window.take_screenshot();
		break;

	default: // To avoid compiler warnings.
		break;
	}
}

void Game::render(Renderer& renderer)
{
	const auto draw_tr = [&renderer](Model& model, const Vector3& translation, float angle, const Vector3& axis)
	{
		PushTransformation t{renderer, Matrix4::translation(translation)};
		PushTransformation r{renderer, Matrix4::rotation(angle, axis)};
		model.draw(renderer);
	};

	const auto draw_rt = [&renderer](Model& model, const Vector3& translation, float angle, const Vector3& axis)
	{
		PushTransformation r{renderer, Matrix4::rotation(angle, axis)};
		PushTransformation t{renderer, Matrix4::translation(translation)};
		model.draw(renderer);
	};

	const auto center = Rect{renderer.window_size()}.center();

	{
		Push3D projection{renderer, Matrix4::perspective(renderer.window_size(), 35, .5, 256), Matrix4::camera(_position, _rotation)};
		_center_ray = renderer.pixel_ray(center);
		{
			PushTransformation r{renderer, Matrix4::rotation(static_cast<float>(_animation % 24000) / 24000.f * 360.f, { 0, 0, 1 })};

			const auto angle = static_cast<float>(_animation % 3000) / 3000.f * 360.f;

			// X direction.
			draw_tr(_cube, {-5.00,  0,    2.50}, 2 * angle, { 1,  0,  0});
			draw_tr(_cube, {-3.75,  0,    1.75},     angle, {-1,  0,  0});
			draw_tr(_cube, {-2.50,  0,    1.25}, 2 * angle, { 1,  0,  0});
			draw_tr(_cube, {-1.25,  0,    1.00},     angle, {-1,  0,  0});
			draw_tr(_cube, { 1.25,  0,    1.00},     angle, { 1,  0,  0});
			draw_tr(_cube, { 2.50,  0,    1.25}, 2 * angle, {-1,  0,  0});
			draw_tr(_cube, { 3.75,  0,    1.75},     angle, { 1,  0,  0});
			draw_tr(_cube, { 5.00,  0,    2.50}, 2 * angle, {-1,  0,  0});

			// Y direction.
			draw_tr(_cube, { 0,    -5.00, 2.50}, 2 * angle, { 0,  1,  0});
			draw_tr(_cube, { 0,    -3.75, 1.75},     angle, { 0, -1,  0});
			draw_tr(_cube, { 0,    -2.50, 1.25}, 2 * angle, { 0,  1,  0});
			draw_tr(_cube, { 0,    -1.25, 1.00},     angle, { 0, -1,  0});
			draw_tr(_cube, { 0,     1.25, 1.00},     angle, { 0,  1,  0});
			draw_tr(_cube, { 0,     2.50, 1.25}, 2 * angle, { 0, -1,  0});
			draw_tr(_cube, { 0,     3.75, 1.75},     angle, { 0,  1,  0});
			draw_tr(_cube, { 0,     5.00, 2.50}, 2 * angle, { 0, -1,  0});

			// Z direction.
			draw_tr(_cube, { 0,     0,    1.00}, 2 * angle, { 0,  0,  1});
			draw_tr(_cube, { 0,     0,    2.25},     angle, { 0,  0, -1});
			draw_tr(_cube, { 0,     0,    3.50}, 2 * angle, { 0,  0,  1});
			draw_tr(_cube, { 0,     0,    4.75},     angle, { 0,  0, -1});

			draw_tr(_cube, {-1.25, -1.25, 2.25},     angle, { 0,  0,  1});
			draw_tr(_cube, {-1.25,  1.25, 2.25},     angle, { 0,  0,  1});
			draw_tr(_cube, { 1.25, -1.25, 2.25},     angle, { 0,  0,  1});
			draw_tr(_cube, { 1.25,  1.25, 2.25},     angle, { 0,  0,  1});

			draw_rt(_cube, {-1.25,  0.00, 3.50},    -angle, { 0,  0,  1});
			draw_rt(_cube, { 1.25,  0.00, 3.50},    -angle, { 0,  0,  1});
			draw_rt(_cube, { 0.00, -1.25, 3.50},    -angle, { 0,  0,  1});
			draw_rt(_cube, { 0.00,  1.25, 3.50},    -angle, { 0,  0,  1});
		}
		{
			Vector3 p;
			if (_center_ray.plane_intersection(_board_plane, p) && std::abs(p.x) <= 64 && std::abs(p.y) <= 64) // TODO-17: Use init-statement.
			{
				_board_point = Vector3{std::floor(p.x) + .5f, std::floor(p.y) + .5f, -.4375f};
				PushTransformation t{renderer, Matrix4::translation(*_board_point)};
				_cube.draw(renderer);
			}
			else
				_board_point = {};
		}
		_checkerboard.draw(renderer);
	}
	{
		PushTexture push_texture{renderer, nullptr};
		renderer.draw_rect(RectF{PointF{center}, SizeF{2, 2}}, {1, 1, 0, 1});
	}
	if (_debug_text_visible)
		renderer.draw_debug_text(_debug_text);
}

void Game::update(const UpdateEvent& update)
{
	_animation += static_cast<unsigned>(update.milliseconds.count());

	if (_move_forward != _move_backward || _move_left != _move_right)
	{
		constexpr auto speed = 8.f; // Units per second.
		const auto distance = static_cast<float>(update.milliseconds.count()) * speed / 1000;
		const auto offset = (_move_forward || _move_backward) && (_move_left || _move_right) ? distance * static_cast<float>(M_SQRT1_2) : distance;

		Vector3 movement{0, 0, 0};
		if (_move_forward)
			movement.y += offset;
		else if (_move_backward)
			movement.y -= offset;
		if (_move_left)
			movement.x -= offset;
		else if (_move_right)
			movement.x += offset;

		_position += Matrix4{_rotation} * movement;

		_position.x = clamp(_position.x, -64.f, 64.f);
		_position.y = clamp(_position.y, -64.f, 64.f);
		_position.z = clamp(_position.z, 1.f, 64.f);
	}

	_debug_text.clear();
	append_to(_debug_text,
		"FPS: ", update.fps, "\n"
		"MaxFrameTime: ", update.max_frame_time.count(), "\n"
		"Triangles: ", update.triangles, "\n"
		"DrawCalls: ", update.draw_calls, "\n"
		"TextureSwitches: ", update.texture_switches, " (Redundant: ", update.redundant_texture_switches, ")\n"
		"ShaderSwitches: ", update.shader_switches, " (Redundant: ", update.redundant_shader_switches, ")\n"
		"X: ", _position.x, ", Y: ", _position.y, ", Z: ", _position.z, "\n"
		"Pitch: ", _rotation._pitch, ", Yaw: ", _rotation._yaw, "\n"
		"Center ray:\n"
		"  ", _center_ray._origin.x, ", ", _center_ray._origin.y, ", ", _center_ray._origin.z, "\n"
		"  ", (_center_ray._origin + _center_ray._vector).x, ", ", (_center_ray._origin + _center_ray._vector).y, ", ", (_center_ray._origin + _center_ray._vector).z, "\n"
		"Board intersection:\n"
		"  ");

	if (_board_point)
		append_to(_debug_text, _board_point->x, ", ", _board_point->y, ", ", _board_point->z);
	else
		append_to(_debug_text, "(none)");
}
