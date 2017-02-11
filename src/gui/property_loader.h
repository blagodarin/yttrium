#ifndef _src_gui_property_loader_h_
#define _src_gui_property_loader_h_

#include <yttrium/renderer/texture.h>
#include "types.h"

namespace Yttrium
{
	class GuiActions;
	class Margins;
	class PointF;
	class RectF;
	template <typename> class ResourcePtr;
	class SizeF;
	class Sound;
	class StaticString;
	class String;
	class Texture2D;
	class TextureFont;
	class Vector4;

	class GuiPropertyLoader
	{
	public:
		virtual ~GuiPropertyLoader() = default;

		// NOTE: The loading functions MAY modify the data even if they haven't finished successfully.
		virtual void bind(const StaticString& name) = 0;
		virtual bool load(const StaticString& name, float&) const = 0;
		virtual GuiActions load_actions(const StaticString& name) const = 0;
		virtual bool load_alignment(const StaticString& name, unsigned*) const = 0;
		virtual bool load_color(const StaticString& name, Vector4*) const = 0;
		virtual void load_font(const StaticString& name, ResourcePtr<const TextureFont>*, ResourcePtr<const Texture2D>*) const = 0;
		virtual bool load_margins(const StaticString& name, Margins&) const = 0;
		virtual bool load_rect(const StaticString& name, RectF&, bool update = false) const = 0;
		virtual ResourcePtr<const Sound> load_sound(const StaticString& name) const = 0;
		virtual bool load_state(const StaticString& name, WidgetState*) const = 0;
		virtual bool load_text(const StaticString& name, String&) const = 0;
		virtual bool load_texture(const StaticString& name, ResourcePtr<const Texture2D>&, Texture2D::Filter&) const = 0;
		virtual bool load_translatable(const StaticString& name, String&) const = 0;
		virtual void unbind() = 0;
	};
}

#endif
