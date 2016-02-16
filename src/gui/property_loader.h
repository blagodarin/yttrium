#ifndef _src_gui_property_loader_h_
#define _src_gui_property_loader_h_

#include "types.h"

namespace Yttrium
{
	class Margins;
	class PointF;
	class RectF;
	class SizeF;
	class Sound;
	class StaticString;
	class String;
	class Texture2D;
	class TextureFont;
	class Vector4;

	template <typename T>
	class SharedPtr;

	class GuiPropertyLoader
	{
	public:
		virtual ~GuiPropertyLoader() = default;

		// NOTE: The loading functions MAY modify the data even if they haven't finished successfully.
		virtual void bind(const StaticString& name) = 0;
		virtual bool load(const StaticString& name, float&) const = 0;
		virtual bool load_alignment(const StaticString& name, unsigned*) const = 0;
		virtual bool load_color(const StaticString& name, Vector4*) const = 0;
		virtual bool load_font(const StaticString& name, TextureFont*, SharedPtr<Texture2D>*) const = 0;
		virtual bool load_margins(const StaticString& name, Margins*) const = 0;
		virtual bool load_rect(const StaticString& name, RectF&, bool update = false) const = 0;
		virtual SharedPtr<Sound> load_sound(const StaticString& name) const = 0;
		virtual bool load_state(const StaticString& name, WidgetState*) const = 0;
		virtual bool load_text(const StaticString& name, String*) const = 0;
		virtual bool load_texture(const StaticString& name, SharedPtr<Texture2D>*) const = 0;
		virtual bool load_translatable(const StaticString& name, String*) const = 0;
		virtual void unbind() = 0;
	};
}

#endif
