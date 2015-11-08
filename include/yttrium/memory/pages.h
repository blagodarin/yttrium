/// \file
/// \brief

#ifndef _include_yttrium_memory_pages_h_
#define _include_yttrium_memory_pages_h_

#include <yttrium/global.h>

#include <cstddef>

namespace Yttrium
{
	/// Page memory allocation.
	class Y_API PageAllocation
	{
	public:

		/// Creates an allocation of no less than the specified size.
		PageAllocation(size_t min_size);

		/// Returns the pointer to the allocated data.
		void* data() noexcept { return _data; }
		const void* data() const noexcept { return _data; }

		/// Returns the actual size of the allocation.
		size_t size() const noexcept { return _size; }

		PageAllocation(const PageAllocation&) = delete;
		PageAllocation(PageAllocation&&) noexcept;
		~PageAllocation() noexcept;
		PageAllocation& operator=(const PageAllocation&) = delete;
		PageAllocation& operator=(PageAllocation&&) noexcept;

	private:
		size_t _size;
		void* _data;
	};
}

#endif
