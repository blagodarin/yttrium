/// \file
/// \brief

#ifndef _include_yttrium_gui_texture_font_h_
#define _include_yttrium_gui_texture_font_h_

#include <yttrium/math/rect.h>

#include <memory>
#include <string>
#include <vector>

namespace Yttrium
{
	class Reader;
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
		/// Returns 'nullptr' for a null Reader.
		/// May throw DataError.
		static std::unique_ptr<TextureFont> open(Reader&&);

		///
		virtual void build(std::vector<TexturedRect>&, const PointF& top_left, float font_size, const std::string&, TextCapture* = nullptr) const = 0;

		///
		virtual Rect rect() const = 0;

		///
		virtual Size text_size(const std::string&) const = 0;

		///
		virtual SizeF text_size(const std::string&, const SizeF& font_size) const = 0;
	};
}

#endif
