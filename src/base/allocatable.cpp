#include <Yttrium/allocators.hpp>

namespace Yttrium
{

void *Allocatable::operator new(size_t size, Allocator *allocator)
{
	Allocator **raw_pointer = static_cast<Allocator **>(allocator->allocate(sizeof(Allocator *) + size));
	*raw_pointer = allocator;
	return raw_pointer + 1;
}

void Allocatable::operator delete(void *pointer) throw()
{
	Allocator **raw_pointer = static_cast<Allocator **>(pointer) - 1;
	(*raw_pointer)->deallocate(raw_pointer);
}

Allocator *Allocatable::allocator() throw()
{
	return *(reinterpret_cast<Allocator **>(dynamic_cast<void *>(this)) - 1);
}

} // namespace Yttrium
