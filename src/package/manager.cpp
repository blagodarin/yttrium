#include <yttrium/package.h>

#include "../base/instance_guard.h"
#include "../memory/private_allocator.h"

namespace Yttrium
{

typedef InstanceGuard<PackageManager> PackageManagerGuard;

class Y_PRIVATE PackageManager::Private
{
public:

	Private(PackageManager *public_, Allocator *allocator)
		: _instance_guard(public_, "Duplicate PackageManager construction")
		, _allocator(allocator)
		, _order(PackageManager::PackedFirst)
	{
	}

	File open_packed(const StaticString &name)
	{
		File file;
		for (auto i = _packages.rbegin(); i != _packages.rend(); ++i)
		{
			file = i->open_file(name);
			if (file.is_opened())
				break;
		}
		return file;
	}

public:

	PackageManagerGuard        _instance_guard;
	PrivateAllocator           _allocator;
	std::vector<PackageReader> _packages;
	Order                      _order;
};

PackageManager::PackageManager(Allocator *allocator)
	: _private(Y_NEW(allocator, PackageManager::Private)(this, allocator))
{
}

PackageManager::~PackageManager()
{
	_private->_allocator.delete_private(_private);
}

bool PackageManager::mount(const StaticString &name, PackageType type)
{
	PackageReader package;

	if (!package.open(name, type, _private->_allocator))
		return false;

	_private->_packages.emplace_back(package);

	return true;
}

File PackageManager::open_file(const StaticString &name, File::Mode mode, Order order)
{
	if (mode != File::Read)
		return File(name, mode, _private->_allocator);

	if (order == PresetOrder)
		order = _private->_order;

	File file;

	if (order == PackedFirst || order == PackedOnly)
	{
		file = _private->open_packed(name);
		if (file.is_opened())
			return file;
	}

	if (order != PackedOnly)
	{
		if (file.open(name, mode, _private->_allocator))
			return file;

		if (order != SystemOnly)
			return _private->open_packed(name);
	}

	return file;
}

void PackageManager::set_order(Order order) noexcept
{
	if (order != PresetOrder)
		_private->_order = order;
}

void PackageManager::unmount_all()
{
	_private->_packages.clear();
}

PackageManager *PackageManager::instance()
{
	return PackageManagerGuard::instance;
}

} // namespace Yttrium
