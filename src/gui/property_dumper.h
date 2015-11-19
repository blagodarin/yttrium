#ifndef _src_gui_property_dumper_h_
#define _src_gui_property_dumper_h_

#include "types.h"

namespace Yttrium
{
	class Margins;
	class PointF;
	class Rect;
	class SizeF;
	class Sound;
	class StaticString;
	class Texture2D;
	class Vector4;

	template <typename T>
	class SharedPtr;

	class GuiPropertyDumper
	{
	public:
		virtual ~GuiPropertyDumper() = default;

		virtual void dump_alignment(const StaticString& name, unsigned) = 0;
		virtual void dump_color(const StaticString& name, const Vector4&) = 0;
		virtual void dump_margins(const StaticString& name, const Margins&) = 0;
		virtual void dump_position(const StaticString& name, const PointF&) = 0;
		virtual void dump_rect(const StaticString& name, const Rect&) = 0;
		virtual void dump_scaling(const StaticString& name, Scaling) = 0;
		virtual void dump_size(const StaticString& name, const SizeF&) = 0;
		virtual void dump_sound(const StaticString& name, const Sound&) = 0;
		virtual void dump_state(const StaticString& name, WidgetState) = 0;
		virtual void dump_text(const StaticString& name, const StaticString&) = 0;
		virtual void dump_texture(const StaticString& name, const SharedPtr<Texture2D>&) = 0;
	};
}

#endif
