#include <yttrium/resources/resource_ptr.h>

namespace Yttrium
{
	static_assert(sizeof(ResourcePtr<Resource>) == sizeof(Resource*), "Bad ResourcePtr size");

	ResourcePtrBase& ResourcePtrBase::operator=(const ResourcePtrBase& other) noexcept
	{
		const auto old_pointer = _pointer;
		_pointer = other._pointer;
		if (_pointer)
			++_pointer->_references;
		if (old_pointer && --old_pointer->_references == 0)
			delete old_pointer;
		return *this;
	}

	ResourcePtrBase& ResourcePtrBase::operator=(ResourcePtrBase&& other) noexcept
	{
		if (_pointer && --_pointer->_references == 0)
			delete _pointer;
		_pointer = other._pointer;
		other._pointer = nullptr;
		return *this;
	}
}
