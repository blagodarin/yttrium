/// \file
/// \brief

#ifndef __Y_TEXTURE_FONT_H
#define __Y_TEXTURE_FONT_H

#include <yttrium/rect.h>
#include <yttrium/static_string.h>

namespace Yttrium
{

/// Texture font markup.
class Y_API TextureFont
{
	Y_DECLARE_PRIVATE(TextureFont);

public:

	/// Font character information.
	struct CharInfo
	{
		Rect rect;    ///< The rectangle a character occupies in the texture.
		Dim2 offset;  ///< The offset to apply before rendering a character.
		int  advance; ///< The horizontal advance.
	};

	///
	explicit TextureFont(const StaticString &name, Allocator *allocator = DefaultAllocator) noexcept
	{
		open(name, allocator);
	}

	///
	const CharInfo *char_info(char symbol) const noexcept;

	///
	int kerning(char left, char right) const noexcept;

	///
	bool open(const StaticString &name, Allocator *allocator = DefaultAllocator) noexcept;

	///
	Rect rect() const noexcept;

	///
	int size() const noexcept;

	///
	Dim2 text_size(const StaticString &text) const noexcept;

	///
	Vector2f text_size(const StaticString &text, const Vector2f &font_size) const noexcept
	{
		const Dim2& size = text_size(text);
		return Vector2f(font_size.x * (size.x * font_size.y / size.y), font_size.y);
	}
};

} // namespace Yttrium

#endif // __Y_TEXTURE_FONT_H
