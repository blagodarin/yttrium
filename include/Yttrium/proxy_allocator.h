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

	ProxyAllocator(const StaticString &name, Allocator *allocator = nullptr) noexcept;

	///

	~ProxyAllocator() noexcept override;

public:

	///

	Allocator *allocator() const noexcept;

	///

	StaticString name() const noexcept;

public: // Allocator.

	using Allocator::allocate;

	///

	void *allocate(size_t size, size_t align = 0, Difference *difference = nullptr) noexcept override;

	///

	void deallocate(void *pointer, Difference *difference = nullptr) noexcept override;

	///

	void *reallocate(void *pointer, size_t size, Movability movability = MayMove, Difference *difference = nullptr) noexcept override;

private:

	class Private;

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_PROXY_ALLOCATOR_H
