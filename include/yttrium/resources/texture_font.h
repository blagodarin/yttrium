/// \file
/// \brief

#ifndef _include_yttrium_resources_texture_font_h_
#define _include_yttrium_resources_texture_font_h_

#include <yttrium/math/rect.h>
#include <yttrium/resources/resource.h>
#include <yttrium/resources/resource_ptr.h>
#include <yttrium/std/vector.h>

#include <memory>

namespace Yttrium
{
	class Reader;
	template <typename> class ResourcePtr;
	class StaticString;
	class TextCapture;
	class TexturedRect;

	/// Texture font markup.
	class Y_API TextureFont : public Resource
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
		static ResourcePtr<TextureFont> load(Reader&&, Allocator&);

		///
		virtual void build(StdVector<TexturedRect>&, const PointF& top_left, float font_size, const StaticString&, TextCapture* = nullptr) const = 0;

		///
		virtual Rect rect() const = 0;

		///
		virtual Size text_size(const StaticString&) const = 0;

		///
		virtual SizeF text_size(const StaticString&, const SizeF& font_size) const = 0;
	};
}

#endif
