#ifndef _src_base_log_h_
#define _src_base_log_h_

#include <yttrium/io/file.h>

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
		File _std_err;
		File _file;
	};

	extern LogManager _log_manager;
}

#endif
