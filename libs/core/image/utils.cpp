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

#include <yttrium/image.h>

#include <cstring>

namespace Yttrium
{
	Image to_bgra(const Image& input)
	{
		const auto input_info = input.info();

		Image output{ { input_info.width(), input_info.height(), PixelFormat::Bgra32, input_info.orientation() } };

		auto src = static_cast<const uint8_t*>(input.data());
		const auto src_row_size = input_info.stride();

		auto dst = static_cast<uint8_t*>(output.data());
		const auto dst_row_size = output.info().stride();

		const auto scanline_size = output.info().width() * 4;

		switch (input_info.pixel_format())
		{
		case PixelFormat::Gray8:
			for (auto y = output.info().height(); y > 0; --y)
			{
				for (size_t a = 0, b = 0; a < scanline_size; a += 4, ++b)
				{
					dst[a + 0] = src[b + 0];
					dst[a + 1] = src[b + 0];
					dst[a + 2] = src[b + 0];
					dst[a + 3] = 0xff;
				}
				src += src_row_size;
				dst += dst_row_size;
			}
			break;

		case PixelFormat::GrayAlpha16:
			for (auto y = output.info().height(); y > 0; --y)
			{
				for (size_t a = 0, b = 0; a < scanline_size; a += 4, b += 2)
				{
					dst[a + 0] = src[b + 0];
					dst[a + 1] = src[b + 0];
					dst[a + 2] = src[b + 0];
					dst[a + 3] = src[b + 1];
				}
				src += src_row_size;
				dst += dst_row_size;
			}
			break;

		case PixelFormat::Rgb24:
			for (auto y = output.info().height(); y > 0; --y)
			{
				for (size_t a = 0, b = 0; a < scanline_size; a += 4, b += 3)
				{
					dst[a + 0] = src[b + 2];
					dst[a + 1] = src[b + 1];
					dst[a + 2] = src[b + 0];
					dst[a + 3] = 0xff;
				}
				src += src_row_size;
				dst += dst_row_size;
			}
			break;

		case PixelFormat::Bgr24:
			for (auto y = output.info().height(); y > 0; --y)
			{
				for (size_t a = 0, b = 0; a < scanline_size; a += 4, b += 3)
				{
					dst[a + 0] = src[b + 0];
					dst[a + 1] = src[b + 1];
					dst[a + 2] = src[b + 2];
					dst[a + 3] = 0xff;
				}
				src += src_row_size;
				dst += dst_row_size;
			}
			break;

		case PixelFormat::Rgba32:
			for (auto y = output.info().height(); y > 0; --y)
			{
				for (size_t a = 0, b = 0; a < scanline_size; a += 4, b += 4)
				{
					dst[a + 0] = src[b + 2];
					dst[a + 1] = src[b + 1];
					dst[a + 2] = src[b + 0];
					dst[a + 3] = src[b + 3];
				}
				src += src_row_size;
				dst += dst_row_size;
			}
			break;

		case PixelFormat::Bgra32:
			std::memcpy(dst, src, input_info.frame_size());
			break;
		}

		return output;
	}

	std::optional<Image> intensity_to_bgra(const Image& input)
	{
		if (input.info().pixel_format() != PixelFormat::Gray8)
			return {};

		Image output{ { input.info().width(), input.info().height(), PixelFormat::Bgra32, input.info().orientation() } };

		auto src = static_cast<const uint8_t*>(input.data());
		const auto src_row_size = input.info().stride();

		auto dst = static_cast<uint8_t*>(output.data());
		const auto dst_row_size = output.info().stride();

		const auto scanline_size = output.info().width() * 4;

		for (auto y = output.info().height(); y > 0; --y)
		{
			for (size_t a = 0, b = 0; a < scanline_size; a += 4, ++b)
			{
				dst[a + 0] = src[b + 0];
				dst[a + 1] = src[b + 0];
				dst[a + 2] = src[b + 0];
				dst[a + 3] = src[b + 0];
			}
			src += src_row_size;
			dst += dst_row_size;
		}

		return std::move(output);
	}
}
