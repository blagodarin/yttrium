//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "game.h"

#include <yttrium/image.h>
#include <yttrium/key.h>
#include <yttrium/math/matrix.h>
#include <yttrium/math/rect.h>
#include <yttrium/renderer/report.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/pass.h>
#include <yttrium/storage/writer.h>
#include <yttrium/utils/string.h>
#include "../../common/include/utils.h"

#include <cmath>

namespace
{
	Yt::Vector3 clamp_position(const Yt::Vector3& v)
	{
		return {
			std::clamp(v.x, -64.f + 12.75f, 64.f - 12.75f),
			std::clamp(v.y, -64.f - 3.5f, 64.f - 17.5f),
			std::clamp(v.z, 1.f, 64.f),
		};
	}
}

class MinimapCanvas : public Yt::Canvas
{
public:
	MinimapCanvas(Yt::Vector3& position, const std::optional<Yt::Quad>& visibility_quad)
		: _position{ position }, _visibility_quad{ visibility_quad } {}

	void on_draw(Yt::RenderPass& pass, const Yt::RectF& rect, std::chrono::milliseconds) override
	{
		Yt::PushTexture push_texture{ pass, nullptr };
		pass.draw_rect(rect, { 0.25, 0.25, 0.25, 0.75 });
		if (_visibility_quad)
			pass.draw_quad(to_window(rect, *_visibility_quad), { 1, 1, 0, 0.25 });
		if (_cursor)
			pass.draw_rect({ *_cursor, Yt::SizeF{ 1, 1 } }, { 0, 1, 0 });
		pass.draw_rect({ to_window(rect, { _position.x, _position.y }) - Yt::Vector2{ 2, 2 }, Yt::SizeF{ 4, 4 } }, { 1, 0, 0 });
	}

	void on_mouse_move(const Yt::RectF& rect, const Yt::Vector2& cursor) override
	{
		_position = ::clamp_position({ to_map(rect, cursor) - Yt::Vector2{ 0, 10 }, _position.z });
		_cursor = cursor;
	}

	bool on_mouse_press(const Yt::RectF& rect, Yt::Key key, const Yt::Vector2& cursor) override
	{
		if (key != Yt::Key::Mouse1)
			return false;
		on_mouse_move(rect, cursor);
		return true;
	}

private:
	static Yt::Vector2 to_map(const Yt::RectF& rect, const Yt::Vector2& v)
	{
		return {
			(v.x - rect.left()) / rect.width() * 128 - 64,
			(rect.top() - v.y) / rect.height() * 128 + 64,
		};
	}

	static Yt::Vector2 to_window(const Yt::RectF& rect, const Yt::Vector2& v)
	{
		return rect.top_left() + Yt::Vector2{ rect.width(), rect.height() } * Yt::Vector2{ v.x + 64, 64 - v.y } / 128;
	}

	static Yt::Quad to_window(const Yt::RectF& rect, const Yt::Quad& q)
	{
		return { to_window(rect, q._a), to_window(rect, q._b), to_window(rect, q._c), to_window(rect, q._d) };
	}

private:
	Yt::Vector3& _position;
	const std::optional<Yt::Quad>& _visibility_quad;
	std::optional<Yt::Vector2> _cursor;
};

Game::Game(const Yt::Storage& storage)
	: _storage{ storage }
	, _minimap_canvas{ std::make_unique<MinimapCanvas>(_position, _visibility_quad) }
{
	_application.on_update([this](std::chrono::milliseconds advance) { update(advance); });

	_script.define("debug", [this](const Yt::ScriptCall&) { _debug_text_visible = !_debug_text_visible; });
	_script.define("screenshot", [this](const Yt::ScriptCall&) { _window.take_screenshot(); });

	_window.on_key_event([this](const Yt::KeyEvent& event) { _gui.process_key_event(event); });
	_window.on_render([this](Yt::RenderPass& pass, const Yt::Vector2& cursor, const Yt::RenderReport& report) {
		draw_scene(pass, cursor);
		_gui.draw(pass, cursor);
		{
			Yt::PushTexture push_texture{ pass, nullptr };
			pass.draw_rect(Yt::RectF{ cursor, Yt::SizeF{ 2, 2 } }, { 1, 1, 0, 1 });
		}
		if (_debug_text_visible)
			draw_debug_text(pass, report);
	});
	_window.on_screenshot([](Yt::Image&& image) { image.save(Yt::Writer{ ::make_screenshot_path() }, Yt::ImageFormat::Png); });

	_gui.bind_canvas("minimap", *_minimap_canvas);
	_gui.on_quit([this] { _window.close(); });
}

Game::~Game() = default;

void Game::run()
{
	_gui.start();
	_window.set_title(_gui.title());
	_window.show();
	_application.run();
}

void Game::draw_debug_text(Yt::RenderPass& pass, const Yt::RenderReport& report)
{
	std::string debug_text;
	Yt::append_to(debug_text,
		"FPS: ", report._fps, '\n',
		"MaxFrameTime: ", report._max_frame_time.count(), " ms\n",
		"Triangles: ", report._triangles, '\n',
		"DrawCalls: ", report._draw_calls, '\n',
		"TextureSwitches: ", report._texture_switches, " (Redundant: ", report._extra_texture_switches, ")\n",
		"ShaderSwitches: ", report._shader_switches, " (Redundant: ", report._extra_shader_switches, ")\n",
		"X: ", _position.x, ", Y: ", _position.y, ", Z: ", _position.z, '\n',
		"Cell: (");
	if (_board_point)
		Yt::append_to(debug_text, static_cast<int>(_board_point->x), ",", static_cast<int>(_board_point->y));
	else
		Yt::append_to(debug_text, "none");
	Yt::append_to(debug_text, ")");
	pass.add_debug_text(debug_text);
}

void Game::draw_scene(Yt::RenderPass& pass, const Yt::Vector2& cursor)
{
	Yt::Push3D projection{ pass, Yt::Matrix4::perspective(pass.window_size(), 35, .5, 256), Yt::Matrix4::camera(_position, _rotation) };
	{
		Yt::Vector3 top_left;
		Yt::Vector3 top_right;
		Yt::Vector3 bottom_left;
		Yt::Vector3 bottom_right;
		const Yt::RectF r{ Yt::SizeF{ pass.window_size() } };
		if (pass.pixel_ray(r.top_left()).plane_intersection(_board_plane, top_left)
			&& pass.pixel_ray(r.top_right()).plane_intersection(_board_plane, top_right)
			&& pass.pixel_ray(r.bottom_left()).plane_intersection(_board_plane, bottom_left)
			&& pass.pixel_ray(r.bottom_right()).plane_intersection(_board_plane, bottom_right))
			_visibility_quad = { { top_left.x, top_left.y }, { top_right.x, top_right.y }, { bottom_right.x, bottom_right.y }, { bottom_left.x, bottom_left.y } };
		else
			_visibility_quad = {};
	}
	_cursor_ray = pass.pixel_ray(cursor);
	if (Yt::Vector3 p; _cursor_ray.plane_intersection(_board_plane, p) && std::abs(p.x) <= 64 && std::abs(p.y) <= 64)
	{
		_board_point.emplace(std::floor(p.x), std::floor(p.y));
		Yt::PushTransformation t{ pass, Yt::Matrix4::translation({ _board_point->x + .5f, _board_point->y + .5f, .5f }) };
		_cube.draw(pass);
	}
	else
		_board_point.reset();
	_checkerboard.draw(pass);
}

void Game::update(std::chrono::milliseconds advance)
{
	const bool move_forward = _window.cursor()._y < 10;
	const bool move_backward = _window.size()._height - _window.cursor()._y <= 10;
	const bool move_left = _window.cursor()._x < 10;
	const bool move_right = _window.size()._width - _window.cursor()._x <= 10;

	if (move_forward != move_backward || move_left != move_right)
	{
		constexpr auto speed = 16.f; // Units per second.
		const auto distance = static_cast<float>(advance.count()) * speed / 1000;
		const auto offset = (move_forward || move_backward) && (move_left || move_right) ? distance * static_cast<float>(M_SQRT1_2) : distance;

		Yt::Vector3 movement{ 0, 0, 0 };
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
}
