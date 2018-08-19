#ifndef _src_platform_posix_temporary_file_h
#define _src_platform_posix_temporary_file_h

#include <yttrium/storage/temporary_file.h>

namespace Yttrium
{
	class TemporaryFilePrivate
	{
	public:
		TemporaryFilePrivate();
		~TemporaryFilePrivate() noexcept;

		static auto descriptor(const TemporaryFile& file) noexcept { return file._private->_descriptor; }

	public:
		std::string _name;
		const int _descriptor;
	};
}

#endif
