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

#pragma once

#include <optional>

namespace Yttrium
{
	class Buffer;
	class ImageInfo;
	enum class ImageType;
	class Reader;
	class Source;
	class Writer;

	std::optional<ImageInfo> read_bmp_header(Reader&);
	std::optional<ImageInfo> read_dds_header(Reader&);
	std::optional<ImageInfo> read_ico_header(Reader&);
#if Y_USE_JPEG
	std::optional<ImageInfo> read_jpeg(const Source&, Buffer&);
#endif
	std::optional<ImageInfo> read_tga_header(Reader&);

#if Y_USE_PNG
	bool write_png(Writer&, const ImageInfo&, const void*);
#endif
	bool write_tga(Writer&, const ImageInfo&, const void*);

	std::optional<ImageInfo> read_image(const Source&, ImageType, Buffer&);
	bool write_image(Writer&, ImageType, const ImageInfo&, const void*);

	bool detect_image_type(const Source&, ImageType&);
}
