//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/storage/writer.h>

#include <catch2/catch.hpp>

using Yttrium::Reader;
using Yttrium::Source;
using Yttrium::TemporaryFile;
using Yttrium::Writer;

TEST_CASE("temporary_file")
{
	TemporaryFile file;
	CHECK(Source::from(file)->size() == 0);
	{
		Writer writer{ file };
		CHECK(writer.size() == 0);
		writer.write(int64_t{ -1 });
		CHECK(writer.size() == sizeof(int64_t));
	}
	{
		const auto source = Source::from(file);
		CHECK(source->size() == sizeof(int64_t));
		Reader reader{ *source };
		int64_t data = 0;
		CHECK(reader.read(data));
		CHECK(data == -1);
	}
	{
		Writer writer{ file };
		CHECK(writer.size() == 0);
	}
	CHECK(Source::from(file)->size() == 0);
}
