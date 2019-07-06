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

#include "../file.h"

#include <yttrium/storage/source.h>
#include "../writer.h"
#include "temporary_file.h"

#include <cstdlib>
#include <system_error>

#include <fcntl.h>
#include <unistd.h>

namespace
{
	auto maybe_size(int descriptor)
	{
		const auto size = ::lseek(descriptor, 0, SEEK_END);
		if (size == -1)
			throw std::system_error{ errno, std::generic_category() };
		return size;
	}
}

namespace Yttrium
{
	class FileSource final : public Source
	{
	public:
		FileSource(uint64_t size, const std::string& name, int descriptor, bool temporary)
			: Source{ size, name }
			, _descriptor{ descriptor }
			, _temporary{ temporary }
		{
		}

		~FileSource() noexcept override
		{
			if (_temporary)
				return;
			if (::close(_descriptor))
				::perror("ERROR! 'close' failed");
		}

		size_t read_at(uint64_t offset, void* data, size_t size) const override
		{
			const auto result = ::pread(_descriptor, data, size, static_cast<int64_t>(offset));
			return result != -1 ? static_cast<size_t>(result) : 0;
		}

	private:
		const int _descriptor;
		const bool _temporary;
	};

	class FileWriter final : public WriterPrivate
	{
	public:
		FileWriter(std::string_view name, int descriptor, bool temporary)
			: _name{ name }
			, _descriptor{ descriptor }
			, _temporary{ temporary }
		{
		}

		~FileWriter() noexcept override
		{
			if (_temporary)
				return;
			if (::close(_descriptor) == -1)
				::perror("ERROR! 'close' failed");
			if (_unlink && ::unlink(_name.c_str()) == -1)
				::perror("ERROR! 'unlink' failed");
		}

		void reserve(uint64_t) override
		{
		}

		void resize(uint64_t size) override
		{
			if (::ftruncate(_descriptor, static_cast<int64_t>(size)) == -1)
				throw std::system_error{ errno, std::generic_category() };
		}

		void unlink() override
		{
			_unlink = true;
		}

		size_t write_at(uint64_t offset, const void* data, size_t size) override
		{
			const auto result = ::pwrite(_descriptor, data, size, static_cast<int64_t>(offset));
			return result != -1 ? static_cast<size_t>(result) : 0;
		}

	private:
		const std::string _name;
		const int _descriptor;
		const bool _temporary;
		bool _unlink = false;
	};

	std::unique_ptr<Source> Source::from(const std::string& path)
	{
#ifdef __linux__
		const int flags = O_RDONLY | O_NOATIME;
#else
		const int flags = O_RDONLY;
#endif
		const auto descriptor = ::open(path.c_str(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (descriptor == -1)
			return {};
		return std::make_unique<FileSource>(::maybe_size(descriptor), path, descriptor, false);
	}

	std::unique_ptr<Source> Source::from(const TemporaryFile& file)
	{
		const auto descriptor = TemporaryFilePrivate::descriptor(file);
		return std::make_unique<FileSource>(::maybe_size(descriptor), file.name(), descriptor, true);
	}

	std::unique_ptr<WriterPrivate> create_file_writer(const std::string& path)
	{
		const auto descriptor = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (descriptor == -1)
			return {};
		return std::make_unique<FileWriter>(path, descriptor, false);
	}

	std::unique_ptr<WriterPrivate> create_file_writer(TemporaryFile& file)
	{
		const auto descriptor = TemporaryFilePrivate::descriptor(file);
		if (::ftruncate(descriptor, 0) == -1)
			throw std::system_error{ errno, std::generic_category() };
		return std::make_unique<FileWriter>(file.name(), descriptor, true);
	}
}
