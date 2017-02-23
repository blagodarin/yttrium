#include "../file.h"

#include <yttrium/storage/temporary_file.h>
#include "../../storage/reader.h"
#include "../../storage/writer.h"

#include <cstdlib>

#include "windows.h"

// TODO: Throw on errors instead of aborting.
// TODO: Report failures during destruction.

namespace Yttrium
{
	class FileReader : public ReaderPrivate
	{
	public:
		FileReader(uint64_t size, const std::string& name, HANDLE handle)
			: ReaderPrivate(size, name)
			, _handle(handle)
		{
		}

		~FileReader() override
		{
			::CloseHandle(_handle);
		}

		size_t read_at(uint64_t offset, void* data, size_t size) const override
		{
			DWORD result = 0;
			OVERLAPPED overlapped = {};
			overlapped.Offset = static_cast<DWORD>(offset);
			overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);
			return ::ReadFile(_handle, data, size, &result, &overlapped) ? result : 0;
		}

	private:
		const HANDLE _handle;
	};

	class FileWriter : public WriterPrivate
	{
	public:
		FileWriter(uint64_t size, const std::string& name, HANDLE handle)
			: WriterPrivate(size)
			, _name(name)
			, _handle(handle)
		{
		}

		~FileWriter() override
		{
			::CloseHandle(_handle);
			if (_unlink)
				::DeleteFileA(_name.text());
		}

		void reserve(uint64_t) override
		{
		}

		void resize(uint64_t size) override
		{
			LARGE_INTEGER offset;
			offset.QuadPart = size;
			if (!::SetFilePointerEx(_handle, offset, nullptr, FILE_BEGIN) || !::SetEndOfFile(_handle)) // TODO: Synchronization?
				std::abort();
		}

		void unlink() override
		{
			_unlink = true;
		}

		size_t write_at(uint64_t offset, const void* data, size_t size) override
		{
			DWORD result = 0;
			OVERLAPPED overlapped = {};
			overlapped.Offset = static_cast<DWORD>(offset);
			overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);
			return ::WriteFile(_handle, data, size, &result, &overlapped) ? result : 0;
		}

	private:
		const std::string _name;
		const HANDLE _handle;
		bool _unlink = false;
	};

	std::shared_ptr<ReaderPrivate> create_file_reader(const std::string& path)
	{
		const auto handle = ::CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (handle == INVALID_HANDLE_VALUE)
			return {};
		LARGE_INTEGER size;
		if (!::GetFileSizeEx(handle, &size))
			std::abort();
		return std::make_shared<FileReader>(size.QuadPart, path, handle);
	}

	std::shared_ptr<ReaderPrivate> create_file_reader(const TemporaryFile& file)
	{
		return create_file_reader(file.name());
	}

	std::unique_ptr<WriterPrivate> create_file_writer(const std::string& path)
	{
		const auto handle = ::CreateFileA(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (handle == INVALID_HANDLE_VALUE)
			return {};
		LARGE_INTEGER size;
		if (!::GetFileSizeEx(handle, &size))
			std::abort();
		return std::make_unique<FileWriter>(size.QuadPart, path, handle);
	}

	std::unique_ptr<WriterPrivate> create_file_writer(TemporaryFile& file)
	{
		return create_file_writer(file.name());
	}
}
