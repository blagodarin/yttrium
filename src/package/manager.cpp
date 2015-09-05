#include <yttrium/package.h>
#include "manager.h"

#include "../base/instance_guard.h"
#include "../memory/private_allocator.h"

#include <vector>

// TODO: Proper mutex protection.

namespace Yttrium
{
	using PackageManagerGuard = InstanceGuard<PackageManager>;

	class Y_PRIVATE PackageManager::Private
	{
	public:

		Private(PackageManager* public_, Allocator* allocator)
			: _allocator(allocator)
			, _instance_guard(public_, "Duplicate PackageManager construction")
		{
		}

		File open_packed(const StaticString& name)
		{
			File file;
			for (auto i = _packages.rbegin(); i != _packages.rend(); ++i)
			{
				file = i->open_file(name);
				if (file)
					break;
			}
			return file;
		}

	public:

		PrivateAllocator           _allocator;
		std::vector<PackageReader> _packages;
		Order                      _order = PackageManager::Order::PackedFirst;
		PackageManagerGuard        _instance_guard;
	};

	PackageManager::PackageManager(Allocator* allocator)
		: _private(Y_NEW(allocator, PackageManager::Private)(this, allocator))
	{
	}

	PackageManager::~PackageManager()
	{
		_private->_allocator.delete_private(_private);
	}

	bool PackageManager::mount(const StaticString& name, PackageType type)
	{
		PackageReader package(name, type, _private->_allocator);
		if (!package)
			return false;
		_private->_packages.emplace_back(package);
		return true;
	}

	File PackageManager::open_file(const StaticString& name, unsigned mode, Order order)
	{
		if (mode != File::Read)
			return File(name, mode, _private->_allocator);

		if (order == Order::Preset)
			order = _private->_order;

		if (order == Order::PackedFirst || order == Order::PackedOnly)
		{
			File&& file = _private->open_packed(name);
			if (file)
				return file;
		}

		if (order != Order::PackedOnly)
		{
			File file(name, mode, _private->_allocator);
			if (file)
				return file;

			if (order != Order::SystemOnly)
				return _private->open_packed(name);
		}

		return {};
	}

	void PackageManager::set_order(Order order)
	{
		if (order != Order::Preset)
			_private->_order = order;
	}

	void PackageManager::unmount_all()
	{
		_private->_packages.clear();
	}

	File open_file_for_reading(const StaticString& name, Allocator* allocator)
	{
		{
			std::lock_guard<std::mutex> lock(PackageManagerGuard::instance_mutex);
			if (PackageManagerGuard::instance)
				return PackageManagerGuard::instance->open_file(name);
		}
		return File(name, File::Read, allocator);
	}
}
