#include <Yttrium/application.hpp>

#include "private.hpp"

// NOTE: We should be GCC-compatible here.

namespace Yttrium
{

static size_t _application_private_references = 0;

Application::Application() throw()
	: _private(Private::pointer())
{
	if (!__sync_fetch_and_add(&_application_private_references, 1))
	{
		new(_private) Private();
	}
}

Application::~Application() throw()
{
	if (!__sync_sub_and_fetch(&_application_private_references, 1))
	{
		_private->Private::~Private();
	}
}

void Application::initialize()
{
	_private->initialize();
}

} // namespace Yttrium
