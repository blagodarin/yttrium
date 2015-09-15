#ifndef __GUI_PROPERTY_LOADER_H
#define __GUI_PROPERTY_LOADER_H

#include "types.h"

namespace Yttrium
{
	class Margins;
	class Rect;
	class Sound;
	class StaticString;
	class String;
	class Texture2D;
	class TextureFont;
	class Vector2;
	class Vector4;

	template <typename T>
	class SharedPtr;

	class GuiPropertyLoader
	{
	public:

		virtual ~GuiPropertyLoader() = default;

		// NOTE: The loading functions MAY modify the data even if they haven't finished successfully.
		virtual void bind(const StaticString& name) = 0;
		virtual bool load_alignment(const StaticString& name, unsigned* alignment) const = 0;
		virtual bool load_color(const StaticString& name, Vector4* color) const = 0;
		virtual bool load_font(const StaticString& name, TextureFont* font, SharedPtr<Texture2D>* texture) const = 0;
		virtual bool load_margins(const StaticString& name, Margins* margins) const = 0;
		virtual bool load_position(const StaticString& name, Vector2* position) const = 0;
		virtual bool load_rect(const StaticString& name, Rect* rect, bool update = false) const = 0;
		virtual bool load_scaling(const StaticString& name, Scaling* scaling) const = 0;
		virtual bool load_size(const StaticString& name, Vector2* size) const = 0;
		virtual SharedPtr<Sound> load_sound(const StaticString& name) const = 0;
		virtual bool load_state(const StaticString& name, WidgetState* state) const = 0;
		virtual bool load_text(const StaticString& name, String* text) const = 0;
		virtual bool load_texture(const StaticString& name, SharedPtr<Texture2D>* texture) const = 0;
		virtual void unbind() = 0;
	};
}

#endif
