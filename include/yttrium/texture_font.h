/// \file
/// \brief Texture font markup.

#ifndef __Y_TEXTURE_FONT_H
#define __Y_TEXTURE_FONT_H

#include <yttrium/base.h>
#include <yttrium/rect.h>
#include <yttrium/types.h>

namespace Yttrium
{
	class StaticString;

	/// Texture font markup.
	class Y_API TextureFont
	{
	public:

		/// Font character information.
		struct CharInfo
		{
			Rect  rect;    ///< The rectangle a character occupies in the texture.
			Point offset;  ///< The offset to apply before rendering a character.
			int   advance; ///< The horizontal advance.
		};

		///
		explicit TextureFont(const StaticString& name, Allocator* allocator = DefaultAllocator);

		///
		const CharInfo* char_info(char symbol) const;

		///
		int kerning(char left, char right) const;

		///
		Rect rect() const;

		///
		int size() const;

		///
		Size text_size(const StaticString& text) const;

		///
		Vector2 text_size(const StaticString& text, const Vector2& font_size) const;

	private:
		Y_SHARED_PRIVATE(TextureFont);
	};
}

#endif // __Y_TEXTURE_FONT_H
