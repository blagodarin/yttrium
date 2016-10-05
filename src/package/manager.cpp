#include <yttrium/package.h>

#include <yttrium/log.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/memory/unique_ptr.h>
#include <yttrium/std/map.h>
#include <yttrium/std/vector.h>
#include "../base/file.h"
#include "../base/instance_guard.h"

#include <cstring>

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
		{
		}

		bool bind(const StaticString& path, Buffer&& buffer) override
		{
			return _bound.emplace(String(path, &_allocator), std::move(buffer)).second; // TODO: Throw if failed to insert.
		}

		bool mount(const StaticString& name, PackageType type) override
		{
			auto package = PackageReader::create(name, type, _allocator);
			if (!package)
			{
				Log() << "("_s << name << ") Unable to open"_s;
				return false;
			}
			_packages.emplace(_packages.begin(), std::move(package));
			return true;
		}

		File open_file(const StaticString& name) const
		{
			const auto open_packed = [this](const StaticString& name) -> File
			{
				for (const auto& package : _packages)
				{
					auto file = package->open_file(name);
					if (file)
						return file;
				}
				return {};
			};

			const auto i = _bound.find(String(name, ByReference()));
			if (i != _bound.end())
				return File(Buffer(i->second.size(), i->second.data()), _allocator); // TODO: Share buffer contents.

			switch (_order)
			{
			case PackageManager::Order::PackedOnly:
				return open_packed(name);

			case PackageManager::Order::PackedFirst:
				{
					auto file = open_packed(name);
					if (file)
						return file;
				}
				return File(name, File::Read, _allocator);

			case PackageManager::Order::SystemFirst:
				{
					File file(name, File::Read, _allocator);
					if (file)
						return file;
				}
				return open_packed(name);

			default:
				return {};
			}
		}

	private:
		Allocator& _allocator;
		const Order _order;
		StdVector<UniquePtr<PackageReader>> _packages{ _allocator };
		StdMap<String, Buffer> _bound{ _allocator };
		PackageManagerGuard _instance_guard{ this, "Duplicate PackageManager construction" };
	};

	UniquePtr<PackageManager> PackageManager::create(Order order, Allocator& allocator)
	{
		return make_unique<PackageManagerImpl>(allocator, order, allocator);
	}

	File::File(const StaticString& path, Allocator& allocator)
	{
		{
			std::lock_guard<std::mutex> lock(PackageManagerGuard::instance_mutex);
			if (PackageManagerGuard::instance)
			{
				_private = PackageManagerGuard::instance->open_file(path)._private;
				return;
			}
		}
		_private = FilePrivate::open(path, Read, allocator);
	}
}
