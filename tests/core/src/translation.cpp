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

#include <yttrium/memory/buffer.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/translation.h>
#include "../../common/include/utils.h"

#include <catch2/catch.hpp>

using Yttrium::Buffer;
using Yttrium::Source;
using Yttrium::TemporaryFile;
using Yttrium::Translation;

TEST_CASE("translation.load")
{
	const auto t = Translation::load(*Source::from(::make_buffer("tr \"hello\" \"Hello\" tr \"world\" \"world!\"")));
	CHECK(t->translate("hello") == "Hello");
	CHECK(t->translate("world") == "world!");
}

TEST_CASE("translation.load.empty")
{
	const auto t = Translation::load(*Source::from(Buffer{}));
	CHECK(t->translate("hello") == "hello");
	CHECK(t->translate("world") == "world");
}

TEST_CASE("translation.remove_obsolete")
{
	const auto t = Translation::load(*Source::from(::make_buffer("tr \"hello\" \"Hello\" tr \"world\" \"world!\"")));
	t->add("world");
	t->remove_obsolete();

	TemporaryFile f;
	REQUIRE(t->save(f.name()));
	CHECK(Source::from(f.name())->to_string() == "tr \"world\" \"world!\"\n");
}

TEST_CASE("translation.save")
{
	const auto t = Translation::load(*Source::from(Buffer{}));
	t->add("hello");
	t->add("world");

	TemporaryFile f;
	REQUIRE(t->save(f.name()));
	CHECK(Source::from(f.name())->to_string() == "tr \"hello\" \"\"\ntr \"world\" \"\"\n");
}

TEST_CASE("translation.save.empty")
{
	const auto t = Translation::load(*Source::from(Buffer{}));

	TemporaryFile f;
	REQUIRE(t->save(f.name()));
	CHECK(Source::from(f.name())->to_string().empty());
}

TEST_CASE("translation.save.invalid")
{
	const auto t = Translation::load(*Source::from(Buffer{}));
	CHECK(!t->save("/inv*lid"));
}
