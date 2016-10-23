/// \file
/// \brief GUI.

#ifndef _include_yttrium_gui_h_
#define _include_yttrium_gui_h_

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
		Gui(ResourceLoader&, ScriptContext&, Allocator& = *DefaultAllocator);

		///
		void clear();

		///
		bool has_layer(const StaticString& name) const;

		///
		bool load(const StaticString& name); // TODO: Throw on failure.

		///
		bool pop_layers(size_t count);

		///
		bool process_key_event(const KeyEvent&);

		///
		bool push_layer(const StaticString& name);

		///
		void render(Renderer&, const PointF& cursor) const;

		///
		void set_canvas_handler(const StaticString& name, const std::function<void(Renderer&, const RectF&)>&);

	public:
		~Gui();

	private:
		const std::unique_ptr<class GuiPrivate> _private;
	};
}

#endif
