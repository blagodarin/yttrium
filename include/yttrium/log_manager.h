/// \file
/// \brief Log manager.

#ifndef _include_yttrium_log_manager_h_
#define _include_yttrium_log_manager_h_

#include <yttrium/types.h>

namespace Yttrium
{
	class Allocator;
	class StaticString;
	template <typename> class Pointer;

	/// Log manager.
	class Y_API LogManager
	{
	public:

		///
		static Pointer<LogManager> create(const StaticString& file_name, Allocator& allocator = *DefaultAllocator);

		virtual ~LogManager() = default;

	protected:
		LogManager() = default;
		LogManager(const LogManager&) = default;
		LogManager& operator=(const LogManager&) = default;
	};
}

#endif
