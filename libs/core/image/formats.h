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
#if Y_USE_JPEG
	bool read_jpeg(const Source&, ImageInfo&, Buffer&);
#endif
	bool read_tga_header(Reader&, ImageInfo&);

	bool write_png(Writer&, const ImageInfo&, const void*);
	bool write_tga(Writer&, const ImageInfo&, const void*);

	bool read_image(const Source&, ImageFormat, ImageInfo&, Buffer&);
	bool write_image(Writer&, ImageFormat, const ImageInfo&, const void*);
}
