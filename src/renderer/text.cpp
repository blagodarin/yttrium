#include <yttrium/text.h>

#include <yttrium/static_string.h>
#include <yttrium/texture_font.h>

namespace Yttrium
{
	void TextGeometry::build(const PointF& top_left, float font_size,
		const StaticString& text, const TextureFont& font, TextCapture* capture)
	{
		_items.clear();
		if (!font)
			return;

		float current_x = top_left.x();
		float current_y = top_left.y();
		char last_symbol = '\0';
		const float scaling = font_size / font.size();

		float selection_left = 0;
		const auto do_capture = [&](unsigned index)
		{
			if (!capture)
				return;

			if (capture->cursor_pos == index)
			{
				capture->cursor_rect = {{current_x, current_y + font_size * 0.125f}, SizeF(2, font_size * 0.75f)};
				capture->has_cursor = true;
			}

			if (capture->selection_begin < capture->selection_end)
			{
				if (index == capture->selection_begin)
				{
					selection_left = current_x;
				}
				else if (index == capture->selection_end)
				{
					capture->selection_rect = {{selection_left, current_y}, PointF(current_x, current_y + font_size)};
					capture->has_selection = true;
				}
			}
		};

		const char* current_symbol = text.text();

		for (size_t i = 0; i < text.size(); ++i, ++current_symbol)
		{
			const TextureFont::CharInfo* info = font.char_info(*current_symbol);

			if (info)
			{
				_items.emplace_back(
					current_x + info->offset.x() * scaling, current_y + info->offset.y() * scaling,
					info->rect.width() * scaling, info->rect.height() * scaling,
					info->rect);

				do_capture(i);

				current_x += (info->advance + font.kerning(last_symbol, *current_symbol)) * scaling;
			}

			last_symbol = *current_symbol;
		}

		do_capture(text.size());
	}
}
