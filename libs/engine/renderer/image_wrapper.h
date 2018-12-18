//
// Copyright 2018 Sergei Blagodarin
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

#include <yttrium/image.h>

namespace Yttrium
{
	class ImageWrapper
	{
	public:
		explicit ImageWrapper(const Image& image)
			: _image{ &image } {}

		const Image* operator->() const noexcept { return _image; }

		void intensity_to_bgra()
		{
			if (auto intensity = Yttrium::intensity_to_bgra(*_image))
			{
				_modified = std::move(intensity);
				_image = &*_modified;
			}
		}

		void to_bgra()
		{
			_modified = Yttrium::to_bgra(*_image);
			_image = &*_modified;
		}

	private:
		const Image* _image;
		std::optional<Image> _modified;
	};
}
