#ifndef _include_yttrium_gui_gui_h_
#define _include_yttrium_gui_gui_h_

#include <yttrium/api.h>

#include <functional>
#include <memory>

namespace Yttrium
{
	class KeyEvent;
	class Music;
	class RectF;
	class Renderer;
	class ResourceLoader;
	class ScriptContext;
	class StaticString;
	class Vector2;

	class Y_API Canvas
	{
	public:
		virtual ~Canvas() = default;
		virtual void on_draw(Renderer&, const RectF&) {}
	};

	class Y_API Gui
	{
	public:
		Gui(ResourceLoader&, ScriptContext&, const StaticString& name);
		~Gui();

		void bind_canvas(const std::string& name, Canvas&);
		void draw(Renderer&, const Vector2& cursor) const;
		void notify(const StaticString& event);
		void on_custom_cursor(const std::function<void(Renderer&, const Vector2&)>&);
		void on_music(const std::function<void(const std::shared_ptr<const Music>&)>&);
		void on_quit(const std::function<void()>&);
		bool process_key_event(const KeyEvent&);
		void start();

	private:
		const std::unique_ptr<class GuiPrivate> _private;
	};
}

#endif
