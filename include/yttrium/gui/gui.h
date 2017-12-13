#ifndef _include_yttrium_gui_gui_h_
#define _include_yttrium_gui_gui_h_

#include <yttrium/api.h>

#include <functional>
#include <memory>
#include <string_view>

namespace Yttrium
{
	enum class Key;
	class KeyEvent;
	class MusicReader;
	class RectF;
	class RenderPass;
	class ResourceLoader;
	class ScriptContext;
	class Vector2;

	class Y_API Canvas
	{
	public:
		virtual ~Canvas() = default;
		virtual void on_draw(RenderPass&, const RectF&) {}
		virtual void on_mouse_move(const RectF&, const Vector2&) {}
		virtual bool on_mouse_press(const RectF&, Key, const Vector2&) { return false; }
	};

	class Y_API Gui
	{
	public:
		Gui(ResourceLoader&, ScriptContext&, std::string_view name);
		~Gui();

		void bind_canvas(const std::string& name, Canvas&);
		void draw(RenderPass&, const Vector2& cursor) const;
		void notify(const std::string& event);
		void on_custom_cursor(const std::function<void(RenderPass&, const Vector2&)>&);
		void on_music(const std::function<void(const std::shared_ptr<MusicReader>&)>&);
		void on_quit(const std::function<void()>&);
		bool process_key_event(const KeyEvent&);
		void start();

	private:
		const std::unique_ptr<class GuiPrivate> _private;
	};
}

#endif
