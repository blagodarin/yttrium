/// \file
/// \brief Proxy allocator.

#ifndef __Y_PROXY_ALLOCATOR_H
#define __Y_PROXY_ALLOCATOR_H

#include <Yttrium/allocator.h>
#include <Yttrium/static_string.h>

namespace Yttrium
{

/// Proxy allocator.

class Y_API ProxyAllocator: public Allocator
{
public:

	///

	ProxyAllocator(Allocator *allocator, const StaticString &name = StaticString()) noexcept;

public:

	///

	inline StaticString name() const noexcept;

public: // Allocator

	///

	virtual void *allocate(size_t size, size_t align = 0, Difference *difference = nullptr) noexcept;

	///

	virtual void deallocate(void *pointer, Difference *difference = nullptr) noexcept;

	///

	virtual void *reallocate(void *pointer, size_t size, Movability movability = MayMove, Difference *difference = nullptr) noexcept;

private:

	Allocator    *_allocator;
	StaticString  _name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StaticString ProxyAllocator::name() const noexcept
{
	return _name;
}

} // namespace Yttrium

#endif // __Y_PROXY_ALLOCATOR_H
