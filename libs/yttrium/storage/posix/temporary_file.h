#pragma once

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
