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

	void bind(const StaticString&, String*, const ScriptArgs&);
	void exit(const StaticString&, String*, const ScriptArgs&);
	void game_pause(const StaticString&, String*, const ScriptArgs&);
	void game_start(const StaticString&, String*, const ScriptArgs&);
	void game_stop(const StaticString&, String*, const ScriptArgs&);
	void game_resume(const StaticString&, String*, const ScriptArgs&);
	void move_down(const StaticString&, String*, const ScriptArgs&);
	void move_left(const StaticString&, String*, const ScriptArgs&);
	void move_right(const StaticString&, String*, const ScriptArgs&);
	void play_music(const StaticString&, String*, const ScriptArgs&);
	void pop_scene(const StaticString&, String*, const ScriptArgs&);
	void push_scene(const StaticString&, String*, const ScriptArgs&);
	void set(const StaticString&, String*, const ScriptArgs&);
	void seta(const StaticString&, String*, const ScriptArgs&);
	void snap(const StaticString&, String*, const ScriptArgs&);
	void stop_music(const StaticString&, String*, const ScriptArgs&);
	void tgcon(const StaticString&, String*, const ScriptArgs&);
	void turn_left(const StaticString&, String*, const ScriptArgs&);
	void turn_right(const StaticString&, String*, const ScriptArgs&);
	void unbindall(const StaticString&, String*, const ScriptArgs&);
	void unset(const StaticString&, String*, const ScriptArgs&);

private:

	Game& _game;
};

#endif // __TETRIUM_COMMANDS_H
