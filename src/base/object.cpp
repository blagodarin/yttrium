#include <Yttrium/assert.hpp>
#include <Yttrium/object.hpp>

namespace Yttrium
{

Object::~Object() throw()
{
	Y_ASSERT(_counter == 0);
}

void Object::increment() throw()
{
	++_counter;
}

void Object::decrement() throw()
{
	Y_ASSERT(_counter > 0);

	if (!--_counter)
	{
		delete this;
	}
}

} // namespace Yttrium
