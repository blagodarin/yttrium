#include <atomic>  // atomic_*
#include <cstddef> // size_t

#include <Yttrium/application.hpp>

#include "private.hpp"

namespace Yttrium
{

namespace
{

std::atomic<size_t> _application_private_references(0);

} // namespace

Application::Application() throw()
	: _private(Private::pointer())
{
	if (!std::atomic_fetch_add<size_t>(&_application_private_references, 1))
	{
		new(_private) Private();
	}
}

Application::~Application() throw()
{
	if (std::atomic_fetch_sub<size_t>(&_application_private_references, 1) == 1)
	{
		_private->Private::~Private();
	}
}

void Application::initialize()
{
	_private->initialize();
}

LogManager Application::log_manager() throw()
{
	return _private->_log_manager_private.log_manager();
}

} // namespace Yttrium
