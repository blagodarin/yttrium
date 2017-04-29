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

class MinimapCanvas : public Canvas
{
public:
	MinimapCanvas(const Vector3& position, const boost::optional<Quad>& visibility_quad)
		: _position{position}, _visibility_quad{visibility_quad} {}

	void on_draw(Renderer& renderer, const RectF& rect) override
	{
		const auto map = [&rect](const Vector2& v)
		{
			return Vector2
			{
				rect.left() + rect.width() * (v.x + 64) / 128,
				rect.top() + rect.height() * (64 - v.y) / 128,
			};
		};

		PushTexture push_texture{renderer, nullptr};
		renderer.draw_rect(rect, {1, 1, 1, 0.25});
		if (_visibility_quad)
			renderer.draw_quad({map(_visibility_quad->_a), map(_visibility_quad->_b), map(_visibility_quad->_c), map(_visibility_quad->_d)}, {1, 1, 0, 0.25});

		const auto camera = map({_position.x, _position.y});
		renderer.draw_rect({{camera.x - 2, camera.y - 2}, SizeF{4, 4}}, {1, 0, 0, 1});
	}

private:
	const Vector3& _position;
	const boost::optional<Quad>& _visibility_quad;
};

Game::Game(const Storage& storage)
	: _storage{storage}
	, _minimap_canvas{std::make_unique<MinimapCanvas>(_position, _visibility_quad)}
{
	_script.define("debug", [this](const ScriptCall&){ _debug_text_visible = !_debug_text_visible; });
	_script.define("screenshot", [this](const ScriptCall&){ _window.take_screenshot(); });

	_window.on_key_event([this](const KeyEvent& event){ _gui.process_key_event(event); });
	_window.on_render([this](Renderer& renderer, const Vector2& cursor)
	{
		draw_scene(renderer, cursor);
		_gui.draw(renderer, cursor);
		{
			PushTexture push_texture{renderer, nullptr};
			renderer.draw_rect(RectF{cursor, SizeF{2, 2}}, {1, 1, 0, 1});
		}
		if (_debug_text_visible)
			renderer.draw_debug_text(_debug_text);
	});
	_window.on_screenshot([this](Image&& image){ image.save(::make_screenshot_path().c_str()); });
	_window.on_update([this](const UpdateEvent& event){ update(event); });

	_gui.bind_canvas("minimap", *_minimap_canvas);
	_gui.on_quit([this]{ _window.close(); });
}

Game::~Game() = default;

void Game::run()
{
	_gui.start();
	_window.show();
	_window.run();
}

void Game::draw_scene(Renderer& renderer, const Vector2& cursor)
{
	Push3D projection{renderer, Matrix4::perspective(renderer.window_size(), 35, .5, 256), Matrix4::camera(_position, _rotation)};
	{
		Vector3 top_left;
		Vector3 top_right;
		Vector3 bottom_left;
		Vector3 bottom_right;
		const RectF r{SizeF{renderer.window_size()}};
		if (renderer.pixel_ray(r.top_left()).plane_intersection(_board_plane, top_left)
			&& renderer.pixel_ray(r.top_right()).plane_intersection(_board_plane, top_right)
			&& renderer.pixel_ray(r.bottom_left()).plane_intersection(_board_plane, bottom_left)
			&& renderer.pixel_ray(r.bottom_right()).plane_intersection(_board_plane, bottom_right))
			_visibility_quad = Quad{{top_left.x, top_left.y}, {top_right.x, top_right.y}, {bottom_right.x, bottom_right.y}, {bottom_left.x, bottom_left.y}};
		else
			_visibility_quad = {};
	}

	_cursor_ray = renderer.pixel_ray(cursor);
	{
		Vector3 p;
		if (_cursor_ray.plane_intersection(_board_plane, p) && std::abs(p.x) <= 64 && std::abs(p.y) <= 64) // TODO-17: Use init-statement.
		{
			_board_point = Point{static_cast<int>(std::floor(p.x)), static_cast<int>(std::floor(p.y))};
			PushTransformation t{renderer, Matrix4::translation({_board_point->_x + .5f, _board_point->_y + .5f, .5f})};
			_cube.draw(renderer);
		}
		else
			_board_point = {};
	}
	_checkerboard.draw(renderer);
}

void Game::update(const UpdateEvent& update)
{
	const bool move_forward = _window.cursor()._y < 10;
	const bool move_backward = _window.size()._height - _window.cursor()._y <= 10;
	const bool move_left = _window.cursor()._x < 10;
	const bool move_right = _window.size()._width - _window.cursor()._x <= 10;

	if (move_forward != move_backward || move_left != move_right)
	{
		constexpr auto speed = 16.f; // Units per second.
		const auto distance = static_cast<float>(update.milliseconds.count()) * speed / 1000;
		const auto offset = (move_forward || move_backward) && (move_left || move_right) ? distance * static_cast<float>(M_SQRT1_2) : distance;

		Vector3 movement{0, 0, 0};
		if (move_forward)
			movement.y += offset;
		else if (move_backward)
			movement.y -= offset;
		if (move_left)
			movement.x -= offset;
		else if (move_right)
			movement.x += offset;

		_position.x = clamp(_position.x + movement.x, -64.f + 12.75f, 64.f - 12.75f);
		_position.y = clamp(_position.y + movement.y, -64.f - 3.5f, 64.f - 17.5f);
		_position.z = clamp(_position.z + movement.z, 1.f, 64.f);
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
		"Cell: (");
	if (_board_point)
		append_to(_debug_text, _board_point->_x, ",", _board_point->_y);
	else
		append_to(_debug_text, "none");
	append_to(_debug_text, ")");
}
