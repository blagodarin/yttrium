#include <Yttrium/package.h>

#include "../base/instance_guard.h"

namespace Yttrium
{

typedef InstanceGuard<PackageManager> PackageManagerGuard;

PackageManager::PackageManager(Allocator *allocator)
	: _allocator(allocator)
	, _order(PackedFirst)
{
	PackageManagerGuard::enter(this, Y_S("Duplicate PackageManager construction"));
}

PackageManager::~PackageManager()
{
	unmount_all();

	PackageManagerGuard::leave(this, Y_S("Unmatched PackageManager destruction"));
}

File PackageManager::open_file(const StaticString &name, File::Mode mode, Order order)
{
	if (mode != File::Read)
	{
		return File(name, mode, _allocator);
	}

	File file;

	if (order == PresetOrder)
	{
		order = _order;
	}

	if (order == PackedFirst || order == PackedOnly)
	{
		file = open_packed(name);
		if (file.is_opened())
		{
			return file;
		}
	}

	if (order != PackedOnly)
	{
		if (file.open(name, mode, _allocator))
		{
			return file;
		}

		if (order != SystemOnly)
		{
			return open_packed(name);
		}
	}

	return file;
}

bool PackageManager::mount(const StaticString &name, PackageType type)
{
	PackageReader package;

	if (!package.open(name, type, _allocator))
	{
		return false;
	}

	_packages.push_back(package);

	return true;
}

void PackageManager::unmount_all()
{
	_packages.clear();
}

PackageManager *PackageManager::instance()
{
	return PackageManagerGuard::instance;
}

File PackageManager::open_packed(const StaticString &name) const
{
	File file;

	for (Packages::reverse_iterator i = _packages.rbegin(); i != _packages.rend(); ++i)
	{
		file = i->open_file(name);

		if (file.is_opened())
		{
			break;
		}
	}

	return file;
}

} // namespace Yttrium
