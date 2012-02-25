#include <Yttrium/object.hpp>

#include <Yttrium/assert.hpp>

namespace Yttrium
{

Object::~Object()
{
	Y_ASSERT(_counter == 0);
}

void Object::increment()
{
	size_t last_counter = _counter++;

	Y_ASSERT(last_counter != SIZE_MAX);
}

void Object::decrement()
{
	size_t last_counter = _counter--;

	Y_ASSERT(last_counter > 0);

	if (last_counter == 1)
	{
		delete this;
	}
}

} // namespace Yttrium
