/// \file
/// \brief

#ifndef _include_yttrium_gui_gui_h_
#define _include_yttrium_gui_gui_h_

#include <yttrium/memory/global.h>

#include <functional>
#include <memory>

namespace Yttrium
{
	class KeyEvent;
	class PointF;
	class RectF;
	class Renderer;
	class ResourceLoader;
	class ScriptContext;
	class StaticString;

	/// GUI manager.
	class Y_API Gui
	{
	public:
		///
		Gui(ResourceLoader&, ScriptContext&, const StaticString& name, Allocator& = *DefaultAllocator);

		///
		~Gui();

		///
		void notify(const StaticString& event);

		///
		bool process_key_event(const KeyEvent&);

		///
		void render(Renderer&, const PointF& cursor) const;

		///
		void set_canvas_handler(const StaticString& name, const std::function<void(Renderer&, const RectF&)>&);

		///
		void set_custom_cursor_handler(const std::function<void(Renderer&, const PointF&)>&);

		///
		void set_quit_handler(const std::function<void()>&);

	private:
		const std::unique_ptr<class GuiPrivate> _private;
	};
}

#endif
