#pragma once

#include "model.h"

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
	Window _window{ _application, "Yttrium 3D example" };
	ResourceLoader _resource_loader{ _storage, &_window.render_manager() };
	ScriptContext _script;
	Gui _gui{ _resource_loader, _script, "examples/3d/data/gui.ion" };
	bool _debug_text_visible = false;
	std::string _debug_text;
	Model _cube{ _resource_loader, "examples/3d/data/cube.obj", "examples/3d/data/cube.material" };
	Model _checkerboard{ _resource_loader, "examples/3d/data/checkerboard.obj", "examples/3d/data/checkerboard.material" };

	Vector3 _position{ 0, -8.5, 16 };
	Euler _rotation{ 0, -60, 0 };

	Line3 _cursor_ray{ { 0, 0, 0 }, { 0, 0, 0 } };
	const Plane _board_plane{ { 0, 0, 1 }, { 0, 0, 0 } };
	std::optional<Vector2> _board_point;
	std::optional<Quad> _visibility_quad;

	std::unique_ptr<MinimapCanvas> _minimap_canvas;
};
