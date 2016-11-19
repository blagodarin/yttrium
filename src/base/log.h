#ifndef _src_base_log_h_
#define _src_base_log_h_

#include <yttrium/storage/writer.h>

#include "std_err.h"

namespace Yttrium
{
	class LogManager
	{
	public:
		LogManager(Allocator&);

		Allocator& allocator() const { return _allocator; }
		void set_file(const StaticString&);
		void write(const StaticString&);

	private:
		Allocator& _allocator;
		StdErr _std_err;
		Writer _writer;
	};

	extern LogManager _log_manager;
}

#endif
