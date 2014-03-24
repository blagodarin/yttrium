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
public:

	/// Font character information.

	struct CharInfo
	{
		Rect rect;    ///< The rectangle a character occupies in the texture.
		Dim2 offset;  ///< The offset to apply before rendering a character.
		int  advance; ///< The horizontal advance.
	};

public:

	///

	inline TextureFont() noexcept;

	///

	inline TextureFont(const StaticString &name, Allocator *allocator = DefaultAllocator) noexcept;

	///

	TextureFont(const TextureFont &font) noexcept;

	///

	inline ~TextureFont() noexcept;

public:

	///

	const CharInfo *char_info(char symbol) const noexcept;

	///

	void close() noexcept;

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

	inline Vector2f text_size(const StaticString &text, const Vector2f &font_size) const noexcept;

public:

	///

	inline operator bool() const noexcept;

	///

	TextureFont &operator =(const TextureFont &font) noexcept;

public:

	class Private;

private:

	Private *_private;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextureFont::TextureFont() noexcept
	: _private(nullptr)
{
}

TextureFont::TextureFont(const StaticString &name, Allocator *allocator) noexcept
	: TextureFont()
{
	open(name, allocator);
}

TextureFont::~TextureFont() noexcept
{
	close();
}

Vector2f TextureFont::text_size(const StaticString &text, const Vector2f &font_size) const noexcept
{
	const Dim2 &size = text_size(text);

	return Vector2f(font_size.x * (size.x * font_size.y / size.y), font_size.y);
}

TextureFont::operator bool() const noexcept
{
	return _private;
}

} // namespace Yttrium

#endif // __Y_TEXTURE_FONT_H
