/// \file
/// \brief Log manager.

#ifndef _include_yttrium_log_manager_h_
#define _include_yttrium_log_manager_h_

#include <yttrium/global.h>

namespace Yttrium
{
	class Allocator;
	class StaticString;
	template <typename> class Pointer;

	///
	class Y_API LogManager
	{
	public:

		///
		static Pointer<LogManager> create(const StaticString& file_name, Allocator* allocator = nullptr);

		virtual ~LogManager() = default;
	};
}

#endif
