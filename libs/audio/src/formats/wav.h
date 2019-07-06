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

#include "../decoder.h"

namespace Yttrium
{
	class WavDecoder final : public AudioDecoder
	{
	public:
		explicit WavDecoder(std::unique_ptr<Source>&&);

		size_t read_frames(void*, size_t) override;
		bool seek_frame(uint64_t) override;

	private:
		uint64_t _data_offset = 0;
	};
}