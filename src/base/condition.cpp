#include "condition.h"

namespace Yttrium
{

Condition::Condition(Allocator *allocator)
	: _private(Y_NEW(allocator, Private)(allocator))
{
}

Condition::Condition(const Condition &condition)
	: _private(Private::copy(condition._private))
{
}

void Condition::close()
{
	Private::release(&_private);
}

Condition &Condition::operator =(const Condition &condition)
{
	Private::assign(&_private, condition._private);

	return *this;
}

} // namespace Yttrium
