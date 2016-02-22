#ifndef _examples_3d_game_h_
#define _examples_3d_game_h_

#include <yttrium/index_buffer.h>
#include <yttrium/math/vector.h>
#include <yttrium/script/context.h>
#include <yttrium/texture_cache.h>
#include <yttrium/vertex_buffer.h>
#include <yttrium/window.h>

#include "model.h"

#include <memory>

using namespace Yttrium;

class Game : public WindowCallbacks
{
public:
	void run();

private:
	void on_cursor_movement(const Point& movement) override;
	void on_key_event(const KeyEvent& event) override;
	void on_render_canvas(Renderer& renderer, const RectF& rect, const StaticString& canvas_name) override;
	void on_update(const UpdateEvent& update) override;

private:
	ScriptContext _script;
	Pointer<Window> _window;
	std::unique_ptr<CubeModel> _cube;
	std::unique_ptr<ChessboardModel> _chessboard;

	Vector4 _position = {-16, -16, 16};
	float _pitch = -30;
	float _yaw = 45;

	bool _move_forward = false;
	bool _move_backward = false;
	bool _move_left = false;
	bool _move_right = false;
};

#endif
