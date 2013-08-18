/// \file
/// \brief System allocator.

#ifndef __Y_SYSTEM_ALLOCATOR_H
#define __Y_SYSTEM_ALLOCATOR_H

#include <yttrium/allocator.h>

namespace Yttrium
{

/// System allocator.

class Y_API SystemAllocator: public Allocator
{
public:

	///

	static SystemAllocator *instance() noexcept;

	///

	static size_t upper_bound(size_t size) noexcept;

public: // Allocator

	using Allocator::allocate;

	void *allocate(size_t size, size_t align = 0, Difference *difference = nullptr) noexcept override;

	void deallocate(void *pointer, Difference *difference = nullptr) noexcept override;

	void *reallocate(void *pointer, size_t size, Movability movability = MayMove, Difference *difference = nullptr) noexcept override;

public:

	class Private;

private:

	Private *_private;

	Y_PRIVATE SystemAllocator();
	Y_PRIVATE ~SystemAllocator() noexcept override;
};

} // namespace Yttrium

#endif // __Y_SYSTEM_ALLOCATOR_H
