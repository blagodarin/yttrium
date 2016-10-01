#ifndef _src_base_posix_file_h_
#define _src_base_posix_file_h_

#include "../file.h"

namespace Yttrium
{
	class SystemFile : public FilePrivate
	{
	public:
		SystemFile(String&& name, unsigned mode, uint64_t size, int descriptor);
		~SystemFile() override;

		bool flush() override;
		size_t read(void*, size_t) override;
		size_t read(void*, size_t, uint64_t) override;
		bool resize(uint64_t) override;
		size_t write(const void*, size_t) override;
		size_t write(const void*, size_t, uint64_t) override;

	private:
		const int _descriptor;
		bool _auto_close = false;
		bool _auto_remove = false;

		friend FilePrivate;
	};
}

#endif
