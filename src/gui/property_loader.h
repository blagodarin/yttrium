#ifndef __GUI_LOADER_H
#define __GUI_LOADER_H

#include "types.h"

#include <yttrium/types.h>

namespace Yttrium
{
	class Sound;
	class StaticString;
	class String;
	class Texture2D;
	class TextureFont;

	template <typename T>
	class Pointer;

	class GuiPropertyLoader
	{
	public:

		virtual ~GuiPropertyLoader() = default;

		// NOTE: The loading functions MAY modify the data even if they haven't finished successfully.
		virtual void bind(const StaticString& name) = 0;
		virtual bool load_alignment(const StaticString& name, unsigned* alignment) const = 0;
		virtual bool load_color(const StaticString& name, Vector4f* color) const = 0;
		virtual bool load_font(const StaticString& name, TextureFont* font, Pointer<Texture2D>* texture) const = 0;
		virtual bool load_margins(const StaticString& name, MarginsI* margins) const = 0;
		virtual bool load_position(const StaticString& name, Vector2f* position) const = 0;
		virtual bool load_rect(const StaticString& name, Rect* rect, bool update = false) const = 0;
		virtual bool load_scaling(const StaticString& name, Scaling* scaling) const = 0;
		virtual bool load_size(const StaticString& name, Vector2f* size) const = 0;
		virtual Pointer<Sound> load_sound(const StaticString& name) const = 0;
		virtual bool load_state(const StaticString& name, WidgetState* state) const = 0;
		virtual bool load_text(const StaticString& name, String* text) const = 0;
		virtual bool load_texture(const StaticString& name, Pointer<Texture2D>* texture) const = 0;
		virtual void unbind() = 0;
	};
}

#endif // __GUI_LOADER_H
