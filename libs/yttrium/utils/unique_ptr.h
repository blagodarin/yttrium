#ifndef _src_utils_unique_ptr_h_
#define _src_utils_unique_ptr_h_

#include <memory>

namespace Yttrium
{
	namespace _
	{
		template <typename T, typename R, typename P, R (*deleter)(P*)>
		struct UniquePtr
		{
			void operator()(T* p)
			{
				if (p)
					deleter(p);
			}

			using Type = std::unique_ptr<T, UniquePtr>;
		};

		template <typename T, typename R, typename P, R (*deleter)(P*)>
		struct UniquePtr<T[], R, P, deleter>
		{
			void operator()(T* p)
			{
				if (p)
					deleter(p);
			}

			using Type = std::unique_ptr<T[], UniquePtr>;
		};

		template <typename R, typename P>
		R result(R (*)(P*));

		template <typename R, typename P>
		P param(R (*)(P*));
	}
}

#define Y_UNIQUE_PTR(type, deleter) \
	Yttrium::_::UniquePtr<type, decltype(Yttrium::_::result(deleter)), decltype(Yttrium::_::param(deleter)), deleter>::Type

#endif
