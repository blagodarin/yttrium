#define _FILE_OFFSET_BITS 64

#include "../file.h"

#include "../../storage/reader.h"
#include "../../storage/writer.h"

#include <cassert>
#include <cstdlib>
#include <limits>
#include <new>
#include <system_error>

#include <fcntl.h>
#include <unistd.h>

namespace Yttrium
{
	class FileReader : public ReaderPrivate
	{
	public:
		FileReader(uint64_t size, const String& name, int descriptor)
			: ReaderPrivate(size, name)
			, _descriptor(descriptor)
		{
		}

		~FileReader() override
		{
			if (::close(_descriptor))
				::perror("ERROR! 'close' failed");
		}

		size_t read_at(uint64_t offset, void* data, size_t size) const override
		{
			const auto result = ::pread(_descriptor, data, size, offset);
			return result != -1 ? result : 0;
		}

	private:
		const int _descriptor;
	};

	class FileWriter : public WriterPrivate
	{
	public:
		FileWriter(uint64_t size, String&& name, int descriptor)
			: WriterPrivate(size)
			, _name(std::move(name))
			, _descriptor(descriptor)
		{
		}

		~FileWriter() override
		{
			if (::close(_descriptor) == -1)
				::perror("ERROR! 'close' failed");
			if (_unlink && ::unlink(_name.text()) == -1)
				::perror("ERROR! 'unlink' failed");
		}

		void reserve(uint64_t) override
		{
		}

		void resize(uint64_t size) override
		{
			if (::ftruncate(_descriptor, size) == -1)
				throw std::system_error(errno, std::generic_category());
		}

		void unlink() override
		{
			_unlink = true;
		}

		size_t write_at(uint64_t offset, const void* data, size_t size) override
		{
			const auto result = ::pwrite(_descriptor, data, size, offset);
			return result != -1 ? result : 0;
		}

	private:
		const String _name;
		const int _descriptor;
		bool _unlink = false;
	};

	std::shared_ptr<ReaderPrivate> create_file_reader(const StaticString& path)
	{
	#if Y_PLATFORM_LINUX
		static_assert(Y_PLATFORM_LINUX, "Include <yttrium/global.h>");
		const int flags = O_RDONLY | O_NOATIME;
	#else
		const int flags = O_RDONLY;
	#endif
		String name(path); // Guaranteed to be null-terminated.
		const auto descriptor = ::open(name.text(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (descriptor == -1)
			return {};
		const auto size = ::lseek(descriptor, 0, SEEK_END);
		if (size == -1)
			throw std::system_error(errno, std::generic_category());
		return std::make_shared<FileReader>(size, std::move(name), descriptor);
	}

	std::unique_ptr<WriterPrivate> create_file_writer(const StaticString& path)
	{
	#if Y_PLATFORM_LINUX
		static_assert(Y_PLATFORM_LINUX, "Include <yttrium/global.h>");
		const int flags = O_WRONLY | O_CREAT | O_TRUNC | O_NOATIME;
	#else
		const int flags = O_WRONLY | O_CREAT | O_TRUNC;
	#endif
		String name(path); // Guaranteed to be null-terminated.
		const auto descriptor = ::open(name.text(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (descriptor == -1)
			return {};
		const auto size = ::lseek(descriptor, 0, SEEK_END);
		if (size == -1)
			throw std::system_error(errno, std::generic_category());
		return std::make_unique<FileWriter>(size, std::move(name), descriptor);
	}
}
