#ifndef __BASE_UTILS_H
#define __BASE_UTILS_H

#include <memory>

namespace Yttrium
{
	namespace _
	{
		template <typename T, typename R, R (*d)(T*)>
		struct UniquePtrDeleter
		{
			void operator()(T* p)
			{
				if (p)
					d(p);
			}
		};
	}
}

#define Y_UNIQUE_PTR(type, deleter) \
	std::unique_ptr<type, Yttrium::_::UniquePtrDeleter<type, std::result_of_t<decltype(&deleter)(type*)>, deleter>>

#endif
