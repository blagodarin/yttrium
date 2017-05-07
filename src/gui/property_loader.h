#ifndef _src_gui_property_loader_h_
#define _src_gui_property_loader_h_

#include <yttrium/renderer/texture.h>
#include <yttrium/std/string_view.h>
#include "types.h"

#include <memory>
#include <string>

namespace Yttrium
{
	class Color4f;
	class GuiActions;
	class Margins;
	class RectF;
	class SizeF;
	class Sound;
	class Texture2D;
	class TextureFont;

	class GuiPropertyLoader
	{
	public:
		virtual ~GuiPropertyLoader() = default;

		// NOTE: The loading functions MAY modify the data even if they haven't finished successfully.
		virtual void bind(std::string_view name) = 0;
		virtual bool load(std::string_view name, float&) const = 0;
		virtual GuiActions load_actions(std::string_view name) const = 0;
		virtual bool load_alignment(std::string_view name, unsigned*) const = 0;
		virtual bool load_color(std::string_view name, Color4f*) const = 0;
		virtual void load_font(std::string_view name, std::shared_ptr<const TextureFont>*, std::shared_ptr<const Texture2D>*) const = 0;
		virtual bool load_margins(std::string_view name, Margins&) const = 0;
		virtual bool load_rect(std::string_view name, RectF&, bool update = false) const = 0;
		virtual std::shared_ptr<const Sound> load_sound(std::string_view name) const = 0;
		virtual bool load_state(std::string_view name, WidgetState*) const = 0;
		virtual bool load_text(std::string_view name, std::string&) const = 0;
		virtual bool load_texture(std::string_view name, std::shared_ptr<const Texture2D>&, Texture2D::Filter&) const = 0;
		virtual bool load_translatable(std::string_view name, std::string&) const = 0;
		virtual void unbind() = 0;
	};
}

#endif
