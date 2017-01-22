/// \file
/// \brief

#ifndef _include_yttrium_gui_gui_h_
#define _include_yttrium_gui_gui_h_

#include <yttrium/memory/global.h>

#include <functional>
#include <memory>

namespace Yttrium
{
	class AudioPlayer;
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
		Gui(ResourceLoader&, ScriptContext&, AudioPlayer&, const StaticString& name, Allocator& = *DefaultAllocator);

		///
		~Gui();

		///
		void notify(const StaticString& event);

		///
		void on_canvas(const std::function<void(Renderer&, const StaticString&, const RectF&)>&);

		///
		void on_custom_cursor(const std::function<void(Renderer&, const PointF&)>&);

		///
		void on_quit(const std::function<void()>&);

		///
		bool process_key_event(const KeyEvent&);

		///
		void render(Renderer&, const PointF& cursor) const;

		///
		void start();

	private:
		const std::unique_ptr<class GuiPrivate> _private;
	};
}

#endif
