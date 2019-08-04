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

#include <system_error>

#include <windows.h>

namespace Yttrium
{
	class FileSource final : public Source
	{
	public:
		FileSource(HANDLE handle, uint64_t size) noexcept
			: _handle{ handle }, _size{ size } {}

		~FileSource() noexcept override
		{
			if (!::CloseHandle(_handle))
				log_last_error("CloseHandle");
		}

		size_t read_at(uint64_t offset, void* data, size_t size) const override
		{
			DWORD result = 0;
			OVERLAPPED overlapped = {};
			overlapped.Offset = static_cast<DWORD>(offset);
			overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);
			return ::ReadFile(_handle, data, static_cast<DWORD>(size), &result, &overlapped) ? result : 0;
		}

		uint64_t size() const noexcept override
		{
			return _size;
		}

	private:
		const HANDLE _handle;
		const uint64_t _size;
	};

	class FileWriter final : public WriterPrivate
	{
	public:
		explicit FileWriter(HANDLE handle) noexcept
			: _handle{ handle } {}

		~FileWriter() noexcept override
		{
			if (!::CloseHandle(_handle))
				log_last_error("CloseHandle");
		}

		void reserve(uint64_t) override
		{
		}

		void resize(uint64_t size) override
		{
			LARGE_INTEGER offset;
			offset.QuadPart = size;
			if (!::SetFilePointerEx(_handle, offset, nullptr, FILE_BEGIN) || !::SetEndOfFile(_handle))
				throw std::system_error{ static_cast<int>(::GetLastError()), std::system_category() };
		}

		size_t write_at(uint64_t offset, const void* data, size_t size) override
		{
			DWORD result = 0;
			OVERLAPPED overlapped = {};
			overlapped.Offset = static_cast<DWORD>(offset);
			overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);
			return ::WriteFile(_handle, data, static_cast<DWORD>(size), &result, &overlapped) ? result : 0;
		}

	private:
		const HANDLE _handle;
	};

	std::unique_ptr<Source> Source::from(const std::filesystem::path& path)
	{
		const auto handle = ::CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (handle == INVALID_HANDLE_VALUE)
		{
			log_last_error("CreateFile");
			return {};
		}
		LARGE_INTEGER size;
		if (!::GetFileSizeEx(handle, &size))
		{
			log_last_error("GetFileSizeEx");
			if (!::CloseHandle(handle))
				log_last_error("CloseHandle");
			return {};
		}
		return std::make_unique<FileSource>(handle, size.QuadPart); // TODO: Fix handle leak on exception.
	}

	std::unique_ptr<Source> Source::from(const TemporaryFile& file)
	{
		return from(file.path());
	}

	std::unique_ptr<WriterPrivate> create_file_writer(const std::filesystem::path& path)
	{
		const auto handle = ::CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (handle == INVALID_HANDLE_VALUE)
		{
			log_last_error("CreateFile");
			return {};
		}
		return std::make_unique<FileWriter>(handle); // TODO: Fix handle leak on exception.
	}

	std::unique_ptr<WriterPrivate> create_file_writer(TemporaryFile& file)
	{
		return create_file_writer(file.path());
	}
}
