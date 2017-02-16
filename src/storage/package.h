#ifndef _src_storage_package_h_
#define _src_storage_package_h_

#include <yttrium/exceptions.h>

namespace Yttrium
{
	class BadPackage : public DataError
	{
	public:
		template <typename... Args>
		BadPackage(Args&&... args) : DataError(std::forward<Args>(args)...) {}
	};
}

#endif
