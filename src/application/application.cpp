#include <Yttrium/application.hpp>

#include "../base/heap_allocator.hpp"

// NOTE: We should be GCC-compatible here.

namespace Yttrium
{

class Application::Private
{
	friend class Application;

public:

	Private()
	{
		Yttrium::_heap_allocator = &_heap_allocator;
	}

	~Private() throw()
	{
		Yttrium::_heap_allocator = NULL;
	}

private:

	HeapAllocatorImpl _heap_allocator;
};

static size_t _application_private_references = 0;
static char   _application_private_buffer[sizeof(Application::Private)] = {};

Application::Application()
	: _private(reinterpret_cast<Private *>(_application_private_buffer))
{
	if (!__sync_fetch_and_add(&_application_private_references, 1))
	{
		try
		{
			new(_private) Private();
		}
		catch (...)
		{
			__sync_sub_and_fetch(&_application_private_references, 1);
			throw;
		}
	}
}

Application::~Application() throw()
{
	if (!__sync_sub_and_fetch(&_application_private_references, 1))
	{
		_private->Private::~Private();
	}
}

} // namespace Yttrium
