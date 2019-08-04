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
#include <yttrium/storage/temporary_file.h>
#include "../../storage/writer.h"
#include "error.h"
#include "temporary_file.h"

#include <cstdlib>
#include <system_error>

#include <fcntl.h>  // open
#include <unistd.h> // close, ftruncate, lseek, pread, pwrite

namespace Yt
{
	class FileSource final : public Source
	{
	public:
		FileSource(int descriptor, uint64_t size, bool proxy) noexcept
			: _descriptor{ descriptor }, _size{ size }, _proxy{ proxy } {}

		~FileSource() noexcept override
		{
			if (!_proxy && ::close(_descriptor) == -1)
				report_errno("close");
		}

		size_t read_at(uint64_t offset, void* data, size_t size) const override
		{
			const auto result = ::pread(_descriptor, data, size, static_cast<int64_t>(offset));
			return result != -1 ? static_cast<size_t>(result) : 0;
		}

		uint64_t size() const noexcept override
		{
			return _size;
		}

	private:
		const int _descriptor;
		const size_t _size;
		const bool _proxy;
	};

	class FileWriter final : public WriterPrivate
	{
	public:
		FileWriter(int descriptor, bool proxy) noexcept
			: _descriptor{ descriptor }, _proxy{ proxy } {}

		~FileWriter() noexcept override
		{
			if (!_proxy && ::close(_descriptor) == -1)
				report_errno("close");
		}

		void reserve(uint64_t) override
		{
		}

		void resize(uint64_t size) override
		{
			if (::ftruncate(_descriptor, static_cast<int64_t>(size)) == -1)
				throw std::system_error{ errno, std::generic_category() };
		}

		size_t write_at(uint64_t offset, const void* data, size_t size) override
		{
			const auto result = ::pwrite(_descriptor, data, size, static_cast<int64_t>(offset));
			return result != -1 ? static_cast<size_t>(result) : 0;
		}

	private:
		const int _descriptor;
		const bool _proxy;
	};

	std::unique_ptr<Source> Source::from(const std::filesystem::path& path)
	{
#ifdef __linux__
		const int flags = O_RDONLY | O_NOATIME;
#else
		const int flags = O_RDONLY;
#endif
		const auto descriptor = ::open(path.c_str(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (descriptor == -1)
		{
			report_errno("open");
			return {};
		}
		const auto size = ::lseek(descriptor, 0, SEEK_END);
		if (size == -1)
		{
			report_errno("lseek");
			if (::close(descriptor) == -1)
				report_errno("close");
			return {};
		}
		return std::make_unique<FileSource>(descriptor, size, false);
	}

	std::unique_ptr<Source> Source::from(const TemporaryFile& file)
	{
		const auto descriptor = TemporaryFilePrivate::descriptor(file);
		const auto size = ::lseek(descriptor, 0, SEEK_END);
		if (size == -1)
		{
			report_errno("lseek");
			return {};
		}
		return std::make_unique<FileSource>(descriptor, size, true);
	}

	std::unique_ptr<WriterPrivate> create_file_writer(const std::filesystem::path& path)
	{
		const auto descriptor = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (descriptor == -1)
		{
			report_errno("open");
			return {};
		}
		return std::make_unique<FileWriter>(descriptor, false);
	}

	std::unique_ptr<WriterPrivate> create_file_writer(TemporaryFile& file)
	{
		const auto descriptor = TemporaryFilePrivate::descriptor(file);
		if (::ftruncate(descriptor, 0) == -1)
		{
			report_errno("ftruncate");
			return {};
		}
		return std::make_unique<FileWriter>(descriptor, true);
	}
}
