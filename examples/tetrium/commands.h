#ifndef __TETRIUM_COMMANDS_H
#define __TETRIUM_COMMANDS_H

namespace Yttrium
{

class ScriptArgs;
class StaticString;
class String;

} // namespace Yttrium

using namespace Yttrium;

class Game;

class Commands // TODO: : public ScriptHandler
{
public:

	Commands(Game& game);

private:

	void bind(const StaticString&, String*, const ScriptArgs&) noexcept;
	void exit(const StaticString&, String*, const ScriptArgs&) noexcept;
	void game_pause(const StaticString&, String*, const ScriptArgs&) noexcept;
	void game_start(const StaticString&, String*, const ScriptArgs&) noexcept;
	void game_stop(const StaticString&, String*, const ScriptArgs&) noexcept;
	void game_resume(const StaticString&, String*, const ScriptArgs&) noexcept;
	void move_down(const StaticString&, String*, const ScriptArgs&) noexcept;
	void move_left(const StaticString&, String*, const ScriptArgs&) noexcept;
	void move_right(const StaticString&, String*, const ScriptArgs&) noexcept;
	void play_music(const StaticString&, String*, const ScriptArgs&) noexcept;
	void pop_scene(const StaticString&, String*, const ScriptArgs&) noexcept;
	void push_scene(const StaticString&, String*, const ScriptArgs&) noexcept;
	void set(const StaticString&, String*, const ScriptArgs&) noexcept;
	void seta(const StaticString&, String*, const ScriptArgs&) noexcept;
	void snap(const StaticString&, String*, const ScriptArgs&) noexcept;
	void stop_music(const StaticString&, String*, const ScriptArgs&) noexcept;
	void tgcon(const StaticString&, String*, const ScriptArgs&) noexcept;
	void turn_left(const StaticString&, String*, const ScriptArgs&) noexcept;
	void turn_right(const StaticString&, String*, const ScriptArgs&) noexcept;
	void unbindall(const StaticString&, String*, const ScriptArgs&) noexcept;
	void unset(const StaticString&, String*, const ScriptArgs&) noexcept;

private:

	Game& _game;
};

#endif // __TETRIUM_COMMANDS_H
