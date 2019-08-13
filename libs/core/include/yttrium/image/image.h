//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#pragma once

#include <yttrium/memory/buffer.h>

#include <optional>

namespace Yt
{
	class Source;
	class Writer;

	/// Pixel format.
	enum class PixelFormat
	{
		Intensity8,  ///< Single channel for both grayscale and alpha.
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
		Jpeg, ///< Joint Photographic Experts Group (JPEG) file format.
		Dds,  ///< DirectDraw Surface (DDS) file format (read only).
		Bmp,  ///< Windows bitmap (BMP) file format (read only).
		Ico,  ///< Windows icon (ICO) file format (read only).
	};

	/// Image orientation.
	enum class ImageOrientation
	{
		XRightYDown, ///< X left-to-right, Y top-to-bottom.
		XRightYUp,   ///< X left-to-right, Y bottom-to-top.
	};

	/// Image format.
	class Y_CORE_API ImageInfo
	{
	public:
		ImageInfo() noexcept = default;
		constexpr ImageInfo(size_t width, size_t height, size_t stride, PixelFormat pixel_format, ImageOrientation orientation = ImageOrientation::XRightYDown) noexcept
			: _width{ width }, _height{ height }, _stride{ stride }, _pixel_format{ pixel_format }, _orientation{ orientation } {}
		constexpr ImageInfo(size_t width, size_t height, PixelFormat pixel_format, ImageOrientation orientation = ImageOrientation::XRightYDown) noexcept
			: ImageInfo{ width, height, stride(width, pixel_format), pixel_format, orientation } {}
		constexpr ImageInfo(size_t width, size_t height, PixelFormat pixel_format, size_t alignment, ImageOrientation orientation = ImageOrientation::XRightYDown) noexcept
			: ImageInfo{ width, height, stride(width, pixel_format, alignment), pixel_format, orientation } {}

		constexpr size_t frame_size() const noexcept { return _stride * _height; }
		constexpr size_t height() const noexcept { return _height; }
		constexpr ImageOrientation orientation() const noexcept { return _orientation; }
		constexpr PixelFormat pixel_format() const noexcept { return _pixel_format; }
		constexpr size_t stride() const noexcept { return _stride; }
		constexpr size_t width() const noexcept { return _width; }

		///
		static constexpr size_t stride(size_t width, PixelFormat format, size_t alignment = 1) noexcept
		{
			return (width * pixel_size(format) + alignment - 1) / alignment * alignment;
		}

		///
		static constexpr size_t pixel_size(PixelFormat format) noexcept
		{
			switch (format)
			{
			case PixelFormat::Intensity8:
			case PixelFormat::Gray8:
				return 1;
			case PixelFormat::GrayAlpha16:
				return 2;
			case PixelFormat::Rgb24:
			case PixelFormat::Bgr24:
				return 3;
			case PixelFormat::Rgba32:
			case PixelFormat::Bgra32:
				return 4;
			}
			return 0;
		}

	private:
		size_t _width;
		size_t _height;
		size_t _stride;
		PixelFormat _pixel_format;
		ImageOrientation _orientation;
	};

	constexpr bool operator==(const ImageInfo& a, const ImageInfo& b) noexcept
	{
		return a.width() == b.width()
			&& a.height() == b.height()
			&& a.stride() == b.stride()
			&& a.pixel_format() == b.pixel_format()
			&& a.orientation() == b.orientation();
	}

	constexpr bool operator!=(const ImageInfo& a, const ImageInfo& b) noexcept { return !(a == b); }

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
		bool save(Writer&&, ImageFormat, int quality = 100) const;

		/// Saves the image to the default screenshot location with an auto-generated name.
		bool save_as_screenshot(ImageFormat, int quality = 100) const;

		///
		Buffer to_buffer(ImageFormat, int quality = 100) const;

		///
		static bool transform(const ImageInfo&, const void* src_data, const ImageInfo&, void* dst_data) noexcept;

	private:
		ImageInfo _info;
		Buffer _buffer;
		Image(const ImageInfo& info, Buffer&& buffer) noexcept
			: _info{ info }, _buffer{ std::move(buffer) } {}
	};
}
