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

	bool read_bmp_header(Reader&, ImageInfo&);
	bool read_dds_header(Reader&, ImageInfo&);
	bool read_ico_header(Reader&, ImageInfo&);

#if YTTRIUM_WITH_JPEG
	bool read_jpeg(const void*, size_t, ImageInfo&, Buffer&);
	bool write_jpeg(Writer&, const ImageInfo&, const void*, int quality);
#endif

	bool write_png(Writer&, const ImageInfo&, const void*);

	bool read_tga_header(Reader&, ImageInfo&);
	bool write_tga(Writer&, const ImageInfo&, const void*);

	bool read_image(const Source&, ImageFormat, ImageInfo&, Buffer&);
	bool write_image(Writer&&, ImageFormat, int quality, const ImageInfo&, const void*);
}