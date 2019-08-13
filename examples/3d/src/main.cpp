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

#include <yttrium/application.h>
#include <yttrium/gui/gui.h>
#include <yttrium/image/image.h>
#include <yttrium/image/utils.h>
#include <yttrium/logger.h>
#include <yttrium/main.h>
#include <yttrium/math/color.h>
#include <yttrium/resource_loader.h>
#include <yttrium/script/context.h>
#include <yttrium/storage/paths.h>
#include <yttrium/storage/storage.h>
#include <yttrium/storage/writer.h>
#include <yttrium/window.h>
#include "game.h"

namespace
{
	void make_checkerboard_texture(Yt::Storage& storage, std::string_view name)
	{
		storage.attach_buffer(name, Yt::make_bgra32_tga(128, 128, [](size_t x, size_t y) {
			return ((x ^ y) & 1) ? Yt::Bgra32{ 0xdd, 0xdd, 0xdd } : Yt::Bgra32{ 0x00, 0x00, 0x00 };
		}));
	}
}

int ymain(int, char**)
{
	Yt::Logger logger;

	Yt::Storage storage{ Yt::Storage::UseFileSystem::Never };
	storage.attach_package("3d.ypq");
	::make_checkerboard_texture(storage, "data/checkerboard.tga");

	Yt::ScriptContext script;

	Yt::Application application;

	Yt::Window window{ application };
	script.define("screenshot", [&window](const Yt::ScriptCall&) { window.take_screenshot(); });
	window.on_screenshot([](Yt::Image&& image) { image.save_as_screenshot(Yt::ImageFormat::Jpeg); });

	Yt::ResourceLoader resource_loader{ storage, &window.render_manager() };

	Yt::Gui gui{ "data/gui.ion", resource_loader, script };
	window.on_key_event([&gui](const Yt::KeyEvent& event) { gui.process_key_event(event); });
	gui.on_quit([&window] { window.close(); });

	Game game{ resource_loader, gui };
	script.define("debug", [&game](const Yt::ScriptCall&) { game.toggle_debug_text(); });
	application.on_update([&window, &game](std::chrono::milliseconds advance) { game.update(window, advance); });
	window.on_render([&gui, &game](Yt::RenderPass& pass, const Yt::Vector2& cursor, const Yt::RenderReport& report) {
		game.draw_scene(pass, cursor);
		gui.draw(pass, cursor);
		game.draw_debug_graphics(pass, cursor, report);
	});

	gui.start();
	window.set_title(gui.title());
	window.show();
	application.run();
	return 0;
}
