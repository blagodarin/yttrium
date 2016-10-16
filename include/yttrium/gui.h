/// \file
/// \brief GUI.

#ifndef _include_yttrium_gui_h_
#define _include_yttrium_gui_h_

#include <yttrium/memory/unique_ptr.h>

#include <functional>

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
		Gui(ResourceLoader&, Renderer&, ScriptContext&, Allocator& = *DefaultAllocator);

		///
		void clear();

		///
		bool has_layer(const StaticString& name) const;

		///
		bool load(const StaticString& filename);

		///
		bool pop_layers(size_t count);

		///
		bool process_key_event(const KeyEvent&);

		///
		bool push_layer(const StaticString& name);

		///
		void render(const PointF& cursor) const;

		///
		void set_canvas_handler(const StaticString& name, const std::function<void(Renderer&, const RectF&)>&);

	private:
		const UniquePtr<class GuiPrivate> _private;

	public:
		~Gui();
	};
}

#endif
