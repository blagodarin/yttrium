#ifndef __GUI_DUMPER_H
#define __GUI_DUMPER_H

#include "types.h"

namespace Yttrium
{
	class Margins;
	class Rect;
	class Sound;
	class StaticString;
	class Texture2D;
	class Vector2;
	class Vector4;

	template <typename T>
	class SharedPtr;

	class GuiPropertyDumper
	{
	public:

		virtual ~GuiPropertyDumper() = default;

		virtual void dump_alignment(const StaticString& name, unsigned alignment) = 0;
		virtual void dump_color(const StaticString& name, const Vector4& color) = 0;
		virtual void dump_margins(const StaticString& name, const Margins& margins) = 0;
		virtual void dump_position(const StaticString& name, const Vector2& position) = 0;
		virtual void dump_rect(const StaticString& name, const Rect& rect) = 0;
		virtual void dump_scaling(const StaticString& name, Scaling scaling) = 0;
		virtual void dump_size(const StaticString& name, const Vector2& size) = 0;
		virtual void dump_sound(const StaticString& name, const Sound& sound) = 0;
		virtual void dump_state(const StaticString& name, WidgetState state) = 0;
		virtual void dump_text(const StaticString& name, const StaticString& text) = 0;
		virtual void dump_texture(const StaticString& name, const SharedPtr<Texture2D>& texture) = 0;
	};
}

#endif // __GUI_DUMPER_H
