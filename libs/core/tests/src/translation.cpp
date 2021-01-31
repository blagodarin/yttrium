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

#include <yttrium/memory/buffer.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/storage/writer.h>
#include <yttrium/test/utils.h>
#include <yttrium/translation.h>

#include <doctest.h>

TEST_CASE("translation.load")
{
	const auto translation = Yt::Translation::load(*Yt::Source::from(::make_buffer("tr \"hello\" \"Hello\" tr \"world\" \"world!\"")));
	CHECK(translation->translate("hello") == "Hello");
	CHECK(translation->translate("world") == "world!");
}

TEST_CASE("translation.load.empty")
{
	const auto translation = Yt::Translation::load(*Yt::Source::from(Yt::Buffer{}));
	CHECK(translation->translate("hello") == "hello");
	CHECK(translation->translate("world") == "world");
}

TEST_CASE("translation.remove_obsolete")
{
	const auto translation = Yt::Translation::load(*Yt::Source::from(::make_buffer("tr \"hello\" \"Hello\" tr \"world\" \"world!\"")));
	translation->add("world");
	translation->remove_obsolete();

	Yt::TemporaryFile file;
	translation->save(Yt::Writer{ file });
	CHECK(Yt::Source::from(file)->to_string() == "tr \"world\" \"world!\"\n");
}

TEST_CASE("translation.save")
{
	const auto translation = Yt::Translation::load(*Yt::Source::from(Yt::Buffer{}));
	translation->add("hello");
	translation->add("world");

	Yt::TemporaryFile file;
	translation->save(Yt::Writer{ file });
	CHECK(Yt::Source::from(file)->to_string() == "tr \"hello\" \"\"\ntr \"world\" \"\"\n");
}

TEST_CASE("translation.save.empty")
{
	const auto translation = Yt::Translation::load(*Yt::Source::from(Yt::Buffer{}));

	Yt::TemporaryFile file;
	translation->save(Yt::Writer{ file });
	CHECK(Yt::Source::from(file)->to_string().empty());
}
