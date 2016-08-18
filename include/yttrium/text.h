/// \file
/// \brief

#ifndef _include_yttrium_text_h_
#define _include_yttrium_text_h_

#include <yttrium/math/rect.h>
#include <yttrium/std/vector.h>

namespace Yttrium
{
	class StaticString;
	class TextureFont;

	///
	class TextCapture
	{
	public:
		const unsigned cursor_pos;
		bool           has_cursor = false;
		RectF          cursor_rect;

		const unsigned selection_begin;
		const unsigned selection_end;
		bool           has_selection = false;
		RectF          selection_rect;

		TextCapture(unsigned cursor_pos, unsigned selection_begin, unsigned selection_size)
			: cursor_pos(cursor_pos)
			, selection_begin(selection_begin)
			, selection_end(selection_begin + selection_size)
		{
		}
	};

	///
	class Y_API TextGeometry
	{
	public:
		///
		struct Item
		{
			RectF target; ///<
			RectF source; ///<

			Item() = default;
			Item(float x, float y, float width, float height, const Rect& source)
				: target({x, y}, SizeF(width, height))
				, source(source)
			{
			}
		};

		TextGeometry(Allocator& allocator) : _items(allocator) {}

		void build(const PointF& top_left, float font_size, const StaticString& text, const TextureFont&, TextCapture* = nullptr);
		const StdVector<Item>& items() const { return _items; }

	private:
		StdVector<Item> _items;
	};
}

#endif
