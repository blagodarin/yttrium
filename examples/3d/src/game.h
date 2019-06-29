//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <yttrium/application.h>
#include <yttrium/gui/gui.h>
#include <yttrium/math/euler.h>
#include <yttrium/math/line.h>
#include <yttrium/math/point.h>
#include <yttrium/math/quad.h>
#include <yttrium/resource_loader.h>
#include <yttrium/script/context.h>
#include <yttrium/storage/storage.h>
#include <yttrium/window.h>
#include "model.h"

#include <optional>

using namespace Yttrium;

class MinimapCanvas;

class Game
{
public:
	explicit Game(const Storage&);
	~Game();

	void run();

private:
	void draw_minimap(RenderPass&, const RectF&);
	void draw_scene(RenderPass&, const Vector2&);
	void update(const UpdateEvent&);

private:
	const Storage& _storage;
	Application _application;
	Window _window{ _application };
	ResourceLoader _resource_loader{ _storage, &_window.render_manager() };
	ScriptContext _script;
	Gui _gui{ "data/gui.ion", _resource_loader, _script };
	bool _debug_text_visible = false;
	std::string _debug_text;
	Model _cube{ _resource_loader, "data/cube.obj", "data/cube.material" };
	Model _checkerboard{ _resource_loader, "data/checkerboard.obj", "data/checkerboard.material" };

	Vector3 _position{ 0, -8.5, 16 };
	Euler _rotation{ 0, -60, 0 };

	Line3 _cursor_ray{ { 0, 0, 0 }, { 0, 0, 0 } };
	const Plane _board_plane{ { 0, 0, 1 }, { 0, 0, 0 } };
	std::optional<Vector2> _board_point;
	std::optional<Quad> _visibility_quad;

	std::unique_ptr<MinimapCanvas> _minimap_canvas;
};
