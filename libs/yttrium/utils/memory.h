#ifndef _libs_yttrium_utils_memory_h_
#define _libs_yttrium_utils_memory_h_

#include <memory>

namespace Yttrium
{
	template <auto Deleter>
	struct UniquePtrDeleter
	{
		template <typename T>
		void operator()(T* p) noexcept
		{
			if (p)
				Deleter(p);
		}
	};

	template <typename T, auto Deleter>
	using UniquePtr = std::unique_ptr<T, UniquePtrDeleter<Deleter>>;
}

#endif
