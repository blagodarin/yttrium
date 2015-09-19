#ifndef _src_base_posix_file_h_
#define _src_base_posix_file_h_

#include <yttrium/file.h>

#include <yttrium/string.h>
#include "../private_base.h"

namespace Yttrium
{
	class Y_PRIVATE File::Private: public PrivateBase<File::Private>
	{
	public:

		const int      _descriptor;
		const unsigned _mode;
		const String   _name;
		bool           _auto_close;
		bool           _auto_remove;

	public:

		Private(String&& name, int descriptor, unsigned mode, Allocator* allocator);
		~Private();

	public:

		static int open(const StaticString& name, int flags);
	};
}

#endif
