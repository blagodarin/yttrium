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

#ifndef _src_storage_formats_ypq_h_
#define _src_storage_formats_ypq_h_

#include <yttrium/memory/buffer.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/writer.h>

#include <vector>

namespace Yttrium
{
	class YpqReader final : public PackageReader
	{
	public:
		explicit YpqReader(std::unique_ptr<Source>&&);
		~YpqReader() override;

		const std::vector<std::string_view>& names() const override { return _names; }
		std::unique_ptr<Source> open(std::size_t) const override;

	private:
		struct Entry;

		const std::shared_ptr<const Source> _source;
		Buffer _metadata_buffer;
		std::vector<std::string_view> _names;
		std::vector<Entry> _entries;
	};

	class YpqWriter final : public PackageWriter
	{
	public:
		explicit YpqWriter(Writer&&);
		~YpqWriter() override;

		bool add(const std::string&) override;
		bool commit() override;

	private:
		struct Entry;

		Writer _writer;
		std::vector<Entry> _entries;
		bool _committed = false;
		bool _finished = false;
	};
}

#endif
