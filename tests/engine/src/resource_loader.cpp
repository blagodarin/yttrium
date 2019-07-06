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

#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/resource_loader.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/storage.h>
#include "../../common/include/utils.h"

#include <catch2/catch.hpp>

using Yttrium::DataError;
using Yttrium::ResourceError;
using Yttrium::ResourceLoader;
using Yttrium::Storage;
using Yttrium::Translation;

const std::string_view _translation_data = "tr \"foo\" \"bar\"";

TEST_CASE("resource_loader.load")
{
	Storage storage{ Storage::UseFileSystem::Never };
	storage.attach_buffer("one", ::make_buffer(_translation_data));
	storage.attach_buffer("two", ::make_buffer(_translation_data));
	storage.attach_buffer("bad", ::make_buffer("\"bad\""));

	ResourceLoader resource_loader{ storage };
	const auto one = resource_loader.load_translation("one");
	CHECK(one);
	CHECK(one == resource_loader.load_translation("one"));
	CHECK(one != resource_loader.load_translation("two"));
	CHECK(resource_loader.load_translation("two") == resource_loader.load_translation("two"));
	CHECK_THROWS_AS(resource_loader.load_translation("three"), ResourceError);
	CHECK_THROWS_AS(resource_loader.load_translation("bad"), DataError);

	CHECK(!resource_loader.load_material("material"));
	CHECK(!resource_loader.load_mesh("mesh"));
	CHECK(!resource_loader.load_texture_2d("texture_2d"));
}

TEST_CASE("resource_loader.open")
{
	Storage storage{ Storage::UseFileSystem::Never };
	storage.attach_buffer("one", ::make_buffer(_translation_data));

	ResourceLoader resource_loader{ storage };
	CHECK(resource_loader.open("one"));
	CHECK_THROWS_AS(resource_loader.open("two"), ResourceError);
}

TEST_CASE("resource_loader.release_unused")
{
	Storage storage{ Storage::UseFileSystem::Never };
	storage.attach_buffer("translation", ::make_buffer(_translation_data));

	ResourceLoader resource_loader{ storage };
	const std::weak_ptr<const Translation> translation{ resource_loader.load_translation("translation") };
	CHECK(!translation.expired());
	resource_loader.release_unused();
	CHECK(translation.expired());
}
