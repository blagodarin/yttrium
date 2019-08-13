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

#include <yttrium/image/image.h>
#include <yttrium/logger.h>
#include <yttrium/storage/writer.h>
#include "../formats.h"

#include <cassert>
#include <csetjmp>

#include <jpeglib.h>

static_assert(BITS_IN_JSAMPLE == 8);

namespace
{
	class JpegCompressor
	{
	public:
		explicit JpegCompressor(Yt::Writer& writer)
			: _writer{ writer }
		{
			_destination.init_destination = reset_buffer;
			_destination.empty_output_buffer = flush_all_buffer;
			_destination.term_destination = flush_buffer;
			::jpeg_std_error(&_error);
			_error.error_exit = handle_error;
			_error.output_message = log_message;
		}

		~JpegCompressor() noexcept
		{
			if (_created && !setjmp(_jmp_buf))
				::jpeg_destroy_compress(&_compress);
		}

		bool compress(const Yt::ImageInfo& info, const void* data) noexcept
		{
			assert(!_created);
			if (info.orientation() != Yt::ImageOrientation::XRightYDown && info.orientation() != Yt::ImageOrientation::XRightYUp)
				return false;
			if (info.width() > JPEG_MAX_DIMENSION || info.height() > JPEG_MAX_DIMENSION)
				return false;
			if (setjmp(_jmp_buf))
				return false;
			::jpeg_create_compress(&_compress);
			_created = true;
			_compress.err = &_error;
			_compress.client_data = this;
			_compress.dest = &_destination;
			_compress.image_width = static_cast<JDIMENSION>(info.width());
			_compress.image_height = static_cast<JDIMENSION>(info.height());
			switch (info.pixel_format())
			{
			case Yt::PixelFormat::Gray8:
				_compress.input_components = 1;
				_compress.in_color_space = JCS_GRAYSCALE;
				break;
			case Yt::PixelFormat::Rgb24:
				_compress.input_components = 3;
				_compress.in_color_space = JCS_EXT_RGB;
				break;
			case Yt::PixelFormat::Bgr24:
				_compress.input_components = 3;
				_compress.in_color_space = JCS_EXT_BGR;
				break;
			case Yt::PixelFormat::Rgba32:
				_compress.input_components = 4;
				_compress.in_color_space = JCS_EXT_RGBX;
				break;
			case Yt::PixelFormat::Bgra32:
				_compress.input_components = 4;
				_compress.in_color_space = JCS_EXT_BGRX;
				break;
			default:
				return false;
			}
			::jpeg_set_defaults(&_compress);
			_compress.optimize_coding = TRUE;
			_compress.dct_method = JDCT_ISLOW;
			::jpeg_set_quality(&_compress, 100, TRUE);
			::jpeg_start_compress(&_compress, TRUE);
			if (info.orientation() == Yt::ImageOrientation::XRightYDown)
				for (auto row = static_cast<const JSAMPLE*>(data); _compress.next_scanline < _compress.image_height; row += info.stride())
					::jpeg_write_scanlines(&_compress, const_cast<JSAMPLE**>(&row), 1);
			else
				for (auto row = static_cast<const JSAMPLE*>(data) + info.frame_size(); _compress.next_scanline < _compress.image_height; row -= info.stride())
					::jpeg_write_scanlines(&_compress, const_cast<JSAMPLE**>(&row), 1);
			::jpeg_finish_compress(&_compress);
			return true;
		}

	private:
		[[noreturn]] static void handle_error(jpeg_common_struct* common) noexcept
		{
			std::longjmp(reinterpret_cast<JpegCompressor*>(common->client_data)->_jmp_buf, 1);
		}

		static void log_message(jpeg_common_struct* common) noexcept
		{
			char buffer[JMSG_LENGTH_MAX];
			common->err->format_message(common, buffer);
			Yt::Logger::log(buffer);
		}

		static void reset_buffer(jpeg_compress_struct* compress) noexcept
		{
			const auto compressor = reinterpret_cast<JpegCompressor*>(compress->client_data);
			compress->dest->next_output_byte = static_cast<JOCTET*>(compressor->_buffer.data());
			compress->dest->free_in_buffer = compressor->_buffer.size();
		}

		static boolean flush_all_buffer(jpeg_compress_struct* compress) noexcept
		{
			const auto compressor = reinterpret_cast<JpegCompressor*>(compress->client_data);
			if (!compressor->_writer.write_all(compressor->_buffer))
				compress->err->error_exit(reinterpret_cast<jpeg_common_struct*>(compress));
			compress->dest->init_destination(compress);
			return TRUE;
		}

		static void flush_buffer(jpeg_compress_struct* compress) noexcept
		{
			const auto compressor = reinterpret_cast<JpegCompressor*>(compress->client_data);
			if (!compressor->_writer.write_all(compressor->_buffer.data(), compressor->_buffer.size() - compress->dest->free_in_buffer))
				compress->err->error_exit(reinterpret_cast<jpeg_common_struct*>(compress));
		}

	private:
		Yt::Writer& _writer;
		std::jmp_buf _jmp_buf;
		jpeg_error_mgr _error;
		jpeg_destination_mgr _destination;
		jpeg_compress_struct _compress;
		bool _created = false;
		Yt::Buffer _buffer{ 65'536 };
	};

	struct JpegErrorHandler
	{
		jpeg_error_mgr _error_mgr;
		std::jmp_buf _jmp_buf;
	};
}

namespace Yt
{
	bool read_jpeg(const void* data, size_t size, ImageInfo& info, Buffer& buffer)
	{
		if (size > std::numeric_limits<unsigned long>::max())
			return false;

		JpegErrorHandler error_handler;
		error_handler._error_mgr.error_exit = [](jpeg_common_struct* cinfo) { std::longjmp(reinterpret_cast<JpegErrorHandler*>(cinfo->err)->_jmp_buf, 1); };
		if (setjmp(error_handler._jmp_buf))
			return false;

		jpeg_decompress_struct decompressor;
		decompressor.err = ::jpeg_std_error(&error_handler._error_mgr);
		::jpeg_create_decompress(&decompressor);
		if (setjmp(error_handler._jmp_buf))
		{
			::jpeg_destroy_decompress(&decompressor);
			return false;
		}

		::jpeg_mem_src(&decompressor, static_cast<const unsigned char*>(data), static_cast<unsigned long>(size));

		::jpeg_read_header(&decompressor, TRUE);

		decompressor.out_color_space = JCS_EXT_BGRA;

		::jpeg_calc_output_dimensions(&decompressor);

		info = { decompressor.output_width, decompressor.output_height, Yt::PixelFormat::Bgra32 };

		try
		{
			buffer.reset(info.frame_size());
		}
		catch (const std::bad_alloc&)
		{
			::jpeg_destroy_decompress(&decompressor);
			throw;
		}

		decompressor.do_fancy_upsampling = TRUE;
		::jpeg_start_decompress(&decompressor);
		for (auto scanline = &buffer[0]; decompressor.output_scanline < decompressor.output_height; scanline += info.stride())
			::jpeg_read_scanlines(&decompressor, &scanline, 1);
		::jpeg_finish_decompress(&decompressor);

		if (setjmp(error_handler._jmp_buf))
			return false;

		::jpeg_destroy_decompress(&decompressor);
		return true;
	}

	bool write_jpeg(Writer& writer, const ImageInfo& info, const void* data)
	{
		return JpegCompressor{ writer }.compress(info, data);
	}
}
