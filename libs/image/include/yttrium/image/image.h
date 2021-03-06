// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/base/buffer.h>

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
	class ImageInfo
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

	///
	class Image
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
		bool save(Writer&&, ImageFormat, int compression = 0) const;

		/// Saves the image to the default screenshot location with an auto-generated name.
		bool save_as_screenshot(ImageFormat, int compression = 0) const;

		///
		Buffer to_buffer(ImageFormat, int compression = 0) const;

		///
		template <typename Callback>
		static Image generateBgra32(size_t width, size_t height, Callback&& callback)
		{
			Image image{ { width, height, PixelFormat::Bgra32 } };
			for (size_t y = 0; y < height; ++y)
			{
				for (size_t x = 0; x < width; ++x)
				{
					const auto pixel = static_cast<uint8_t*>(image.data()) + y * image.info().stride() + x * 4;
					const auto color = callback(x, y);
					pixel[0] = color._b;
					pixel[1] = color._g;
					pixel[2] = color._r;
					pixel[3] = color._a;
				}
			}
			return image;
		}

		///
		static bool transform(const ImageInfo&, const void* src_data, const ImageInfo&, void* dst_data) noexcept;

	private:
		ImageInfo _info;
		Buffer _buffer;
		Image(const ImageInfo& info, Buffer&& buffer) noexcept
			: _info{ info }, _buffer{ std::move(buffer) } {}
	};
}
