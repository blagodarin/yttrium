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

#include <chrono>
#include <memory>

namespace Yt
{
	class Gui;
	class RenderPass;
	struct RenderReport;
	class ResourceLoader;
	class Window;
	class Vector2;
}

class GameState;
class MinimapCanvas;
class WorldCanvas;

class Game
{
public:
	Game(Yt::ResourceLoader&, Yt::Gui&);
	~Game() noexcept;

	void draw_debug_graphics(Yt::RenderPass&, const Yt::Vector2& cursor, const Yt::RenderReport&);
	void toggle_debug_text() noexcept;
	void update(const Yt::Window&, std::chrono::milliseconds);

private:
	const std::unique_ptr<GameState> _state;
	const std::unique_ptr<WorldCanvas> _world;
	const std::unique_ptr<MinimapCanvas> _minimap;
};
