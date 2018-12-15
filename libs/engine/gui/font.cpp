//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/gui/font.h>

#include <yttrium/exceptions.h>
#include <yttrium/image.h>
#include <yttrium/math/size.h>
#include <yttrium/storage/source.h>

#include <cassert>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Yttrium
{
	class FontPrivate
	{
	public:
		FontPrivate()
		{
			if (FT_Init_FreeType(&_freetype))
				throw InitializationError{ "Failed to initialize FreeType library" };
		}

		~FontPrivate() noexcept
		{
			if (_font_face)
				FT_Done_Face(_font_face);
			FT_Done_FreeType(_freetype); // TODO: Handle error code.
		}

		void load(Buffer&& buffer)
		{
			assert(!_buffer.capacity());
			if (buffer.size() > std::numeric_limits<FT_Long>::max()
				|| FT_New_Memory_Face(_freetype, static_cast<const FT_Byte*>(buffer.data()), static_cast<FT_Long>(buffer.size()), 0, &_font_face))
				throw DataError{ "Failed to load font" };
		}

	private:
		FT_Library _freetype = nullptr;
		Buffer _buffer;
		FT_Face _font_face = nullptr;
	};

	Font::Font(const Source& source)
		: _private{ std::make_unique<FontPrivate>() }
	{
		_private->load(source.to_buffer());
	}

	Font::~Font() noexcept = default;

	void Font::build(std::vector<TexturedRect>&, const Vector2&, float, std::string_view, TextCapture*) const
	{
	}

	Image Font::image() const
	{
		return Image{ { 0, 0, PixelFormat::Gray8 } };
	}

	Size Font::text_size(std::string_view) const
	{
		return {};
	}

	SizeF Font::text_size(std::string_view, const SizeF&) const
	{
		return {};
	}
}
