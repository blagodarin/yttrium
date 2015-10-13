#include <yttrium/package.h>
#include "manager.h"

#include <yttrium/file.h>
#include "../base/instance_guard.h"

#include <vector>

namespace Yttrium
{
	class PackageManagerImpl;

	using PackageManagerGuard = InstanceGuard<PackageManagerImpl>;

	class PackageManagerImpl : public PackageManager
	{
	public:

		PackageManagerImpl(Order order, Allocator& allocator)
			: _allocator(allocator)
			, _order(order)
			, _instance_guard(this, "Duplicate PackageManager construction")
		{
		}

		bool mount(const StaticString& name, PackageType type) override
		{
			auto&& package = PackageReader::create(name, type, _allocator);
			if (!package)
				return false;
			_packages.emplace(_packages.begin(), std::move(package));
			return true;
		}

		File open_file(const StaticString& name) const
		{
			const auto open_packed = [this](const StaticString& name) -> File
			{
				for (const auto& package : _packages)
				{
					File&& file = package->open_file(name);
					if (file)
						return std::move(file);
				}
				return {};
			};

			switch (_order)
			{
			case PackageManager::Order::PackedOnly:
				return open_packed(name);

			case PackageManager::Order::PackedFirst:
				{
					auto&& file = open_packed(name);
					if (file)
						return std::move(file);
				}
				return File(name, File::Read, &_allocator);

			case PackageManager::Order::SystemFirst:
				{
					File file(name, File::Read, &_allocator);
					if (file)
						return std::move(file);
				}
				return open_packed(name);

			default:
				return {};
			}
		}

	private:

		Allocator& _allocator;
		std::vector<Pointer<PackageReader>> _packages;
		const Order _order = PackageManager::Order::PackedFirst;
		PackageManagerGuard _instance_guard;
	};

	Pointer<PackageManager> PackageManager::create(Order order, Allocator& allocator)
	{
		return make_pointer<PackageManagerImpl>(allocator, order, allocator);
	}

	File open_file_for_reading(const StaticString& name, Allocator& allocator)
	{
		{
			std::lock_guard<std::mutex> lock(PackageManagerGuard::instance_mutex);
			if (PackageManagerGuard::instance)
				return PackageManagerGuard::instance->open_file(name);
		}
		return File(name, File::Read, &allocator);
	}
}
