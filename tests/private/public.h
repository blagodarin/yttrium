#ifndef __PUBLIC_H
#define __PUBLIC_H

#include <Yttrium/allocator.h>

class Public
{
public:

	inline Public();

	Public(const Public &public_);

	inline ~Public();

public:

	void close();

	void open(Yttrium::Allocator *allocator = Yttrium::DefaultAllocator);

public:

	Public &operator =(const Public &public_);

public:

	class Private;

public:

	Private *_private;
};

////////////////////////////////////////////////////////////////////////////////

Public::Public()
	: _private(nullptr)
{
}

Public::~Public()
{
	close();
}

#endif // __PUBLIC_H
