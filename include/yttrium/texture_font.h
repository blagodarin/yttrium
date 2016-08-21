/// \file
/// \brief Texture font markup.

#ifndef _include_yttrium_texture_font_h_
#define _include_yttrium_texture_font_h_

#include <yttrium/base.h>
#include <yttrium/math/rect.h>
#include <yttrium/memory/global.h>
#include <yttrium/std/vector.h>

namespace Yttrium
{
	class StaticString;
	class TextCapture;
	class TexturedRect;

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
		void build(StdVector<TexturedRect>&, const PointF& top_left, float font_size, const StaticString&, const SizeF& texture_size, TextCapture* = nullptr) const;

		///
		Rect rect() const;

		///
		int size() const;

		///
		Size text_size(const StaticString& text) const;

		///
		SizeF text_size(const StaticString& text, const SizeF& font_size) const;

	private:
		Y_SHARED_PRIVATE(TextureFont);
	};
}

#endif
