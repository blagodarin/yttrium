//
// Copyright 2019 Sergei Blagodarin
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

#ifndef _include_yttrium_image_h_
#define _include_yttrium_image_h_

#include <yttrium/memory/buffer.h>

#include <optional>

namespace Yttrium
{
	class Source;
	class Writer;

	/// Pixel format.
	enum class PixelFormat
	{
		Gray8,       ///< Gray.
		GrayAlpha16, ///< Gray-alpha.
		Rgb24,       ///< Red-green-blue.
		Bgr24,       ///< Blue-green-red (reverse).
		Rgba32,      ///< Red-green-blue-alpha.
		Bgra32,      ///< Blue-green-red-alpha (reverse RGB).
	};

	/// Image format.
	enum class ImageFormat
	{
		Auto, ///< Automatic detection.
		Tga,  ///< Truevision TARGA (TGA) file format.
		Png,  ///< Portable Network Graphics (PNG) file format (write only).
		Jpeg, ///< Joint Photographic Experts Group (JPEG) file format (read only).
		Dds,  ///< DirectDraw Surface (DDS) file format (read only).
		Bmp,  ///< Windows bitmap (BMP) file format (read only).
		Ico,  ///< Windows icon (ICO) file format (read only).
	};

	/// Image orientation.
	enum class ImageOrientation
	{
		XRightYDown, ///< X left-to-right, Y top-to-bottom.
		XRightYUp,   ///< X left-to-right, Y bottom-to-top.
		XLeftYDown,  ///< X right-to-left, Y top-to-bottom.
		XLeftYUp,    ///< X right-to-left, Y bottom-to-top.
	};

	/// Image format.
	class Y_CORE_API ImageInfo
	{
	public:
		///
		ImageInfo(std::size_t width, std::size_t height, PixelFormat, std::size_t row_alignment = 1, ImageOrientation = ImageOrientation::XRightYDown);

		///
		ImageInfo(std::size_t width, std::size_t height, PixelFormat pixel_format, ImageOrientation orientation)
			: ImageInfo{ width, height, pixel_format, 1, orientation } {}

		std::size_t frame_size() const noexcept { return _row_size * _height; }
		std::size_t height() const noexcept { return _height; }
		ImageOrientation orientation() const noexcept { return _orientation; }
		PixelFormat pixel_format() const noexcept { return _pixel_format; }
		std::size_t pixel_size() const noexcept { return pixel_size(_pixel_format); }
		std::size_t row_alignment() const noexcept { return _row_alignment; }
		std::size_t row_size() const noexcept { return _row_size; }
		std::size_t width() const noexcept { return _width; }

		///
		static constexpr std::size_t pixel_size(PixelFormat format) noexcept
		{
			switch (format)
			{
			case PixelFormat::Gray8: return 1;
			case PixelFormat::GrayAlpha16: return 2;
			case PixelFormat::Rgb24: return 3;
			case PixelFormat::Bgr24: return 3;
			case PixelFormat::Rgba32: return 4;
			case PixelFormat::Bgra32: return 4;
			}
			return 0;
		}

	private:
		PixelFormat _pixel_format;
		ImageOrientation _orientation;
		std::size_t _width;
		std::size_t _row_alignment;
		std::size_t _row_size;
		std::size_t _height;
		friend class Image;
	};

	inline bool operator==(const ImageInfo& a, const ImageInfo& b) noexcept
	{
		return a.pixel_format() == b.pixel_format()
			&& a.orientation() == b.orientation()
			&& a.width() == b.width()
			&& a.row_size() == b.row_size()
			&& a.height() == b.height();
	}

	inline bool operator!=(const ImageInfo& a, const ImageInfo& b) noexcept { return !(a == b); }

	///
	class Y_CORE_API Image
	{
	public:
		///
		static std::optional<Image> load(const Source&, ImageFormat = ImageFormat::Auto);

		/// Creates an image with uninitialized contents.
		explicit Image(const ImageInfo&);

		/// Creates an image initialized from the provided data.
		Image(const ImageInfo&, const void* data);

		///
		void* data() noexcept { return _buffer.data(); }
		const void* data() const noexcept { return _buffer.data(); }

		///
		const ImageInfo& info() const noexcept { return _info; }

		///
		void flip_vertically();

		///
		bool save(Writer&&, ImageFormat) const;

		///
		bool swap_channels() noexcept;

		///
		Buffer to_buffer(ImageFormat) const;

	private:
		ImageInfo _info;
		Buffer _buffer;
		Image(const ImageInfo& info, Buffer&& buffer) noexcept
			: _info{ info }, _buffer{ std::move(buffer) } {}
	};

	///
	Y_CORE_API Image to_bgra(const Image&);

	///
	Y_CORE_API std::optional<Image> intensity_to_bgra(const Image&);
}

#endif
