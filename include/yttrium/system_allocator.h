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

	static SystemAllocator *instance();

	///

	static size_t upper_bound(size_t size);

public: // Allocator

	using Allocator::allocate;

	void *allocate(size_t size, size_t align = 0, Difference *difference = nullptr) override;

	void deallocate(void *pointer, Difference *difference = nullptr) override;

	void *reallocate(void *pointer, size_t size, Movability movability = MayMove, Difference *difference = nullptr) override;

public:

	class Private;

private:

	Private *_private;

	Y_PRIVATE SystemAllocator();
	Y_PRIVATE ~SystemAllocator() override;
};

} // namespace Yttrium

#endif // __Y_SYSTEM_ALLOCATOR_H
