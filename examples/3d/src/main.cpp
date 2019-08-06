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

#include <yttrium/logger.h>
#include <yttrium/main.h>
#include "../../common/include/utils.h"
#include "game.h"

namespace
{
	void make_checkerboard_texture(Yt::Storage& storage, const std::string& name)
	{
		storage.attach_buffer(name, ::make_bgra_tga(128, 128, [](size_t x, size_t y) {
			return ((x ^ y) & 1) ? Yt::Bgra32{ 0xdd, 0xdd, 0xdd } : Yt::Bgra32{ 0x00, 0x00, 0x00 };
		}));
	}
}

int ymain(int, char**)
{
	Yt::Logger logger;
	Yt::Storage storage{ Yt::Storage::UseFileSystem::Never };
	storage.attach_package("3d.ypq");
	::make_checkerboard_texture(storage, "data/checkerboard.tga");

	Game game{ storage };
	game.run();
	return 0;
}
