#ifndef __GUI_DUMPER_H
#define __GUI_DUMPER_H

#include "types.h"

#include <yttrium/types.h>

namespace Yttrium
{
	class Sound;
	class StaticString;
	class Texture2D;

	template <typename T>
	class Pointer;

	class GuiPropertyDumper
	{
	public:

		virtual ~GuiPropertyDumper() = default;

		virtual void dump_alignment(const StaticString& name, unsigned alignment) = 0;
		virtual void dump_color(const StaticString& name, const Vector4f& color) = 0;
		virtual void dump_margins(const StaticString& name, const MarginsI& margins) = 0;
		virtual void dump_position(const StaticString& name, const Vector2f& position) = 0;
		virtual void dump_rect(const StaticString& name, const Rect& rect) = 0;
		virtual void dump_scaling(const StaticString& name, Scaling scaling) = 0;
		virtual void dump_size(const StaticString& name, const Vector2f& size) = 0;
		virtual void dump_sound(const StaticString& name, const Sound& sound) = 0;
		virtual void dump_state(const StaticString& name, WidgetState state) = 0;
		virtual void dump_text(const StaticString& name, const StaticString& text) = 0;
		virtual void dump_texture(const StaticString& name, const Pointer<Texture2D>& texture) = 0;
	};
}

#endif // __GUI_DUMPER_H
