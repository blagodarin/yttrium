#ifndef _examples_tetrium_game_h_
#define _examples_tetrium_game_h_

#include <yttrium/audio/manager.h>
#include <yttrium/audio/player.h>
#include <yttrium/gui/gui.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/script/context.h>
#include <yttrium/static_string.h>
#include <yttrium/window.h>

#include "cursor.h"
#include "graphics.h"

#include <map>

using namespace Yttrium;

class Game : public WindowCallbacks
{
public:
	Game(Storage&);
	~Game();

	void run();

private:
	// WindowCallbacks
	void on_key_event(const KeyEvent&) override;
	void on_render(Renderer&, const PointF& cursor) override;
	void on_screenshot(Image&&) override;
	void on_update(const UpdateEvent&) override;

	void update_statistics();

private:
	const Storage& _storage;
	ScriptContext _script;
	AudioManager _audio;
	AudioPlayer _audio_player{ _audio };
	Window _window{ "Tetrium", *this };
	ResourceLoader _resource_loader{ _storage, &_window.renderer(), &_audio };
	Gui _gui{ _resource_loader, _script, "examples/tetrium/data/gui.ion" };
	Cursor _cursor{ _window.renderer() };
	TetriumGraphics _graphics{ _window.renderer() };
	Tetrium::Game _logic;
	std::multimap<int, std::string> _statistics;
};

#endif
