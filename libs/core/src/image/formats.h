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

#include <yttrium/api.h>

#include <cstddef>

namespace Yttrium
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
	bool read_tga_header(Reader&, ImageInfo&);

	enum class Upsampling
	{
		Nearest,
		Linear,
	};

	Y_CORE_API bool read_jpeg(const void*, std::size_t, ImageInfo&, Buffer&, Upsampling);

	bool write_png(Writer&, const ImageInfo&, const void*);
	bool write_tga(Writer&, const ImageInfo&, const void*);

	bool read_image(const Source&, ImageFormat, ImageInfo&, Buffer&);
	bool write_image(Writer&, ImageFormat, const ImageInfo&, const void*);
}