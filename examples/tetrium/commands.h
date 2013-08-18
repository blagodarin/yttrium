#ifndef __TETRIUM_COMMANDS_H
#define __TETRIUM_COMMANDS_H

#include <yttrium/script/args.h>

using namespace Yttrium;

class Game;

class Commands
{
public:

	Commands(Game *game);

private:

	void bind(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void exit(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void game_pause(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void game_start(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void game_stop(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void game_resume(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void move_down(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void move_left(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void move_right(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void play_music(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void pop_scene(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void push_scene(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void set(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void seta(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void snap(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void stop_music(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void tgcon(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void turn_left(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void turn_right(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void unbindall(const StaticString &name, String *result, const ScriptArgs &args) noexcept;
	void unset(const StaticString &name, String *result, const ScriptArgs &args) noexcept;

private:

	Game *_game;
};

#endif // __TETRIUM_COMMANDS_H
