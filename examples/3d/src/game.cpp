#include "game.h"

#include "../../utils.h"

#include <yttrium/image.h>
#include <yttrium/key.h>
#include <yttrium/math/matrix.h>
#include <yttrium/math/rect.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/pass.h>
#include <yttrium/utils/string.h>

#include <cmath>

namespace
{
	Vector3 clamp_position(const Vector3& v)
	{
		return
		{
			std::clamp(v.x, -64.f + 12.75f, 64.f - 12.75f),
			std::clamp(v.y, -64.f - 3.5f, 64.f - 17.5f),
			std::clamp(v.z, 1.f, 64.f),
		};
	}
}

class MinimapCanvas : public Canvas
{
public:
	MinimapCanvas(Vector3& position, const std::optional<Quad>& visibility_quad)
		: _position{position}, _visibility_quad{visibility_quad} {}

	void on_draw(RenderPass& pass, const RectF& rect) override
	{
		PushTexture push_texture{pass, nullptr};
		pass.draw_rect(rect, {0.25, 0.25, 0.25, 0.75});
		if (_visibility_quad)
			pass.draw_quad(to_window(rect, *_visibility_quad), {1, 1, 0, 0.25});
		if (_cursor)
			pass.draw_rect({*_cursor, SizeF{1, 1}}, {0, 1, 0});
		pass.draw_rect({to_window(rect, {_position.x, _position.y}) - Vector2{2, 2}, SizeF{4, 4}}, {1, 0, 0});
	}

	void on_mouse_move(const RectF& rect, const Vector2& cursor) override
	{
		_position = ::clamp_position({to_map(rect, cursor) - Vector2{0, 10}, _position.z});
		_cursor = cursor;
	}

	bool on_mouse_press(const RectF& rect, Key key, const Vector2& cursor) override
	{
		if (key != Key::Mouse1)
			return false;
		on_mouse_move(rect, cursor);
		return true;
	}

private:
	static Vector2 to_map(const RectF& rect, const Vector2& v)
	{
		return
		{
			(v.x - rect.left()) / rect.width() * 128 - 64,
			(rect.top() - v.y) / rect.height() * 128 + 64,
		};
	}

	static Vector2 to_window(const RectF& rect, const Vector2& v)
	{
		return rect.top_left() + Vector2{rect.width(), rect.height()} * Vector2{v.x + 64, 64 - v.y} / 128;
	}

	static Quad to_window(const RectF& rect, const Quad& q)
	{
		return {to_window(rect, q._a), to_window(rect, q._b), to_window(rect, q._c), to_window(rect, q._d)};
	}

private:
	Vector3& _position;
	const std::optional<Quad>& _visibility_quad;
	std::optional<Vector2> _cursor;
};

Game::Game(const Storage& storage)
	: _storage{storage}
	, _minimap_canvas{std::make_unique<MinimapCanvas>(_position, _visibility_quad)}
{
	_script.define("debug", [this](const ScriptCall&){ _debug_text_visible = !_debug_text_visible; });
	_script.define("screenshot", [this](const ScriptCall&){ _window.take_screenshot(); });

	_window.on_key_event([this](const KeyEvent& event){ _gui.process_key_event(event); });
	_window.on_render([this](RenderPass& pass, const Vector2& cursor)
	{
		draw_scene(pass, cursor);
		_gui.draw(pass, cursor);
		{
			PushTexture push_texture{pass, nullptr};
			pass.draw_rect(RectF{cursor, SizeF{2, 2}}, {1, 1, 0, 1});
		}
		if (_debug_text_visible)
			pass.add_debug_text(_debug_text);
	});
	_window.on_screenshot([](Image&& image){ image.save(::make_screenshot_path()); });
	_window.on_update([this](const UpdateEvent& event){ update(event); });

	_gui.bind_canvas("minimap", *_minimap_canvas);
	_gui.on_quit([this]{ _window.close(); });
}

Game::~Game() = default;

void Game::run()
{
	_gui.start();
	_window.show();
	_application.run();
}

void Game::draw_scene(RenderPass& pass, const Vector2& cursor)
{
	Push3D projection{pass, Matrix4::perspective(pass.window_size(), 35, .5, 256), Matrix4::camera(_position, _rotation)};
	{
		Vector3 top_left;
		Vector3 top_right;
		Vector3 bottom_left;
		Vector3 bottom_right;
		const RectF r{SizeF{pass.window_size()}};
		if (pass.pixel_ray(r.top_left()).plane_intersection(_board_plane, top_left)
			&& pass.pixel_ray(r.top_right()).plane_intersection(_board_plane, top_right)
			&& pass.pixel_ray(r.bottom_left()).plane_intersection(_board_plane, bottom_left)
			&& pass.pixel_ray(r.bottom_right()).plane_intersection(_board_plane, bottom_right))
			_visibility_quad = Quad{{top_left.x, top_left.y}, {top_right.x, top_right.y}, {bottom_right.x, bottom_right.y}, {bottom_left.x, bottom_left.y}};
		else
			_visibility_quad = {};
	}
	_cursor_ray = pass.pixel_ray(cursor);
	if (Vector3 p; _cursor_ray.plane_intersection(_board_plane, p) && std::abs(p.x) <= 64 && std::abs(p.y) <= 64)
	{
		_board_point.emplace(std::floor(p.x), std::floor(p.y));
		PushTransformation t{pass, Matrix4::translation({_board_point->x + .5f, _board_point->y + .5f, .5f})};
		_cube.draw(pass);
	}
	else
		_board_point.reset();
	_checkerboard.draw(pass);
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

		_position = ::clamp_position(_position + movement);
	}

	_debug_text.clear();
	append_to(_debug_text,
		"FPS: ", update.fps.value_or(-1), "\n"
		"MaxFrameTime: ", update.max_frame_time.count(), "\n"
		"Triangles: ", update.triangles, "\n"
		"DrawCalls: ", update.draw_calls, "\n"
		"TextureSwitches: ", update.texture_switches, " (Redundant: ", update.redundant_texture_switches, ")\n"
		"ShaderSwitches: ", update.shader_switches, " (Redundant: ", update.redundant_shader_switches, ")\n"
		"X: ", _position.x, ", Y: ", _position.y, ", Z: ", _position.z, "\n"
		"Cell: (");
	if (_board_point)
		append_to(_debug_text, static_cast<int>(_board_point->x), ",", static_cast<int>(_board_point->y));
	else
		append_to(_debug_text, "none");
	append_to(_debug_text, ")");
}
