#include <Yttrium/object.hpp>

#include <Yttrium/assert.hpp>

namespace Yttrium
{

Object::~Object() noexcept
{
	Y_ASSERT(_counter == 0);
}

void Object::increment() noexcept
{
	++_counter;

	Y_ASSERT(_counter > 0); // Check for counter wrapping.
}

void Object::decrement() noexcept
{
	Y_ASSERT(_counter > 0);

	if (!--_counter)
	{
		delete this;
	}
}

} // namespace Yttrium
