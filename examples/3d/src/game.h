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

#pragma once

#include <yttrium/math/euler.h>
#include <yttrium/math/line.h>
#include <yttrium/math/point.h>
#include <yttrium/math/quad.h>
#include "model.h"

#include <chrono>
#include <optional>

namespace Yt
{
	class Gui;
	struct RenderReport;
	class ResourceLoader;
	class Window;
}

class MinimapCanvas;

class Game
{
public:
	Game(Yt::ResourceLoader&, Yt::Gui&);
	~Game();

	void draw_debug_graphics(Yt::RenderPass&, const Yt::Vector2& cursor, const Yt::RenderReport&);
	void draw_scene(Yt::RenderPass&, const Yt::Vector2&);
	void toggle_debug_text() noexcept { _debug_text_visible = !_debug_text_visible; }
	void update(const Yt::Window&, std::chrono::milliseconds);

private:
	bool _debug_text_visible = false;
	Model _cube;
	Model _checkerboard;

	Yt::Vector3 _position{ 0, -8.5, 16 };
	Yt::Euler _rotation{ 0, -60, 0 };

	Yt::Line3 _cursor_ray{ { 0, 0, 0 }, { 0, 0, 0 } };
	const Yt::Plane _board_plane{ { 0, 0, 1 }, { 0, 0, 0 } };
	std::optional<Yt::Vector2> _board_point;
	std::optional<Yt::Quad> _visibility_quad;

	std::unique_ptr<MinimapCanvas> _minimap_canvas;
};
