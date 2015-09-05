/// \file
/// \brief Log manager.

#ifndef __Y_LOG_MANAGER_H
#define __Y_LOG_MANAGER_H

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
		static Pointer<LogManager> create(const StaticString& file, Allocator* allocator = nullptr);

		virtual ~LogManager() = default;
	};
}

#endif
