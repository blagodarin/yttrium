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

Application::Application()
	: _private(Private::pointer())
{
	if (!std::atomic_fetch_add<size_t>(&_application_private_references, 1))
	{
		new(_private) Private();
	}
}

Application::~Application()
{
	if (std::atomic_fetch_sub<size_t>(&_application_private_references, 1) == 1)
	{
		_private->Private::~Private();
	}
}

} // namespace Yttrium
