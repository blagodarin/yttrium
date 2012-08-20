#ifndef __PRIVATE_H
#define __PRIVATE_H

#include "src/base/private_base.h"

#include "public.h"

class Public::Private: public Yttrium::PrivateBase<Public::Private>
{
public:

	Private(Yttrium::Allocator *allocator)
		: PrivateBase(allocator)
	{
	}
};

#endif // __PRIVATE_H
