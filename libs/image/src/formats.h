// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>

namespace Yt
{
	class Buffer;
	class ImageInfo;
	enum class ImageFormat;
	class Reader;
	class Source;
	class Writer;

#if YTTRIUM_IMAGE_BMP
	bool read_bmp_header(Reader&, ImageInfo&);
#endif
#if YTTRIUM_IMAGE_DDS
	bool read_dds_header(Reader&, ImageInfo&);
#endif
#if YTTRIUM_IMAGE_ICO
	bool read_ico_header(Reader&, ImageInfo&);
#endif
#if YTTRIUM_IMAGE_JPEG
	bool read_jpeg(const void*, size_t, ImageInfo&, Buffer&);
	bool write_jpeg(Writer&, const ImageInfo&, const void*, int compression);
#endif
#if YTTRIUM_IMAGE_PNG
	bool write_png(Writer&, const ImageInfo&, const void*, int compression);
#endif
#if YTTRIUM_IMAGE_TGA
	bool read_tga_header(Reader&, ImageInfo&);
	bool write_tga(Writer&, const ImageInfo&, const void*);
#endif
	bool read_image(const Source&, ImageFormat, ImageInfo&, Buffer&);
	bool write_image(Writer&&, ImageFormat, int compression, const ImageInfo&, const void*);
}
