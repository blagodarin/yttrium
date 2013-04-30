#include "dir.h"

namespace Yttrium
{

Dir::Dir(const Dir &dir)
	: _private(Private::copy(dir._private))
{
}

void Dir::close()
{
	Private::release(&_private);
}

Dir &Dir::operator =(const Dir &dir)
{
	Private::assign(&_private, dir._private);

	return *this;
}

} // namespace Yttrium
