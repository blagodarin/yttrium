#ifndef _include_yttrium_gui_texture_font_h_
#define _include_yttrium_gui_texture_font_h_

#include <yttrium/api.h>
#include <yttrium/math/rect.h>

#include <memory>
#include <string_view>
#include <vector>

namespace Yttrium
{
	class Source;
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
		/// May throw DataError.
		static std::unique_ptr<TextureFont> load(const Source&);

		virtual ~TextureFont() = default;

		///
		virtual void build(std::vector<TexturedRect>&, const Vector2& top_left, float font_size, std::string_view, TextCapture* = nullptr) const = 0;

		///
		virtual Rect rect() const = 0;

		///
		virtual Size text_size(std::string_view) const = 0;

		///
		virtual SizeF text_size(std::string_view, const SizeF& font_size) const = 0;
	};
}

#endif
