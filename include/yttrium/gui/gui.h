/// \file
/// \brief

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

	/// GUI manager.
	class Y_API Gui
	{
	public:
		///
		Gui(ResourceLoader&, ScriptContext&, const StaticString& name);

		///
		~Gui();

		///
		void notify(const StaticString& event);

		///
		void on_canvas(const std::function<void(Renderer&, const std::string&, const RectF&)>&);

		///
		void on_custom_cursor(const std::function<void(Renderer&, const Vector2&)>&);

		///
		void on_music(const std::function<void(const std::shared_ptr<const Music>&)>&);

		///
		void on_quit(const std::function<void()>&);

		///
		bool process_key_event(const KeyEvent&);

		///
		void render(Renderer&, const Vector2& cursor) const;

		///
		void start();

	private:
		const std::unique_ptr<class GuiPrivate> _private;
	};
}

#endif
