#ifndef _src_base_buffer_file_h_
#define _src_base_buffer_file_h_

#include <yttrium/memory/buffer.h>
#include "file.h"

namespace Yttrium
{
	class BufferFile : public FilePrivate
	{
	public:
		BufferFile(String&& name, unsigned mode, Buffer&&);
		~BufferFile() override;

		bool flush() override;
		size_t read(void*, size_t) override;
		size_t read(void*, size_t, uint64_t) override;
		bool resize(uint64_t) override;
		size_t write(const void*, size_t) override;
		size_t write(const void*, size_t, uint64_t) override;

	private:
		Buffer _buffer;
	};
}

#endif
