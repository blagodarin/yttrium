#include <yttrium/resource_manager.h>

#include <yttrium/log.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/std/map.h>
#include <yttrium/std/vector.h>
#include "../base/file.h"
#include "../base/instance_guard.h"

namespace Yttrium
{
	using ResourceManagerGuard = InstanceGuard<ResourceManagerPrivate>;

	class ResourceManagerPrivate
	{
	public:
		ResourceManagerPrivate(ResourceManager::SearchOrder order, Allocator& allocator)
			: _allocator(allocator)
			, _order(order)
		{
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
			case ResourceManager::SearchOrder::PackedOnly:
				return open_packed(name);

			case ResourceManager::SearchOrder::PackedFirst:
				{
					auto file = open_packed(name);
					if (file)
						return file;
				}
				return File(name, File::Read, _allocator);

			case ResourceManager::SearchOrder::SystemFirst:
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
		const ResourceManager::SearchOrder _order;
		StdVector<UniquePtr<PackageReader>> _packages{ _allocator };
		StdMap<String, Buffer> _bound{ _allocator };
		ResourceManagerGuard _instance_guard{ this, "Duplicate ResourceManager construction" };

		friend ResourceManager;
	};

	ResourceManager::ResourceManager(SearchOrder order, Allocator& allocator)
		: _private(make_unique<ResourceManagerPrivate>(allocator, order, allocator))
	{
	}

	bool ResourceManager::bind(const StaticString& path, Buffer&& buffer)
	{
		// TODO: Throw if failed to insert.
		if (!_private->_bound.emplace(String(path, &_private->_allocator), std::move(buffer)).second)
		{
			Log() << "("_s << path << ") Unable to bind"_s;
			return false;
		}
		return true;
	}

	bool ResourceManager::mount_package(const StaticString& path, PackageType type)
	{
		auto package = PackageReader::create(path, type, _private->_allocator);
		if (!package)
		{
			Log() << "("_s << path << ") Unable to open"_s;
			return false;
		}
		_private->_packages.emplace(_private->_packages.begin(), std::move(package));
		return true;
	}

	ResourceManager::~ResourceManager() = default;

	File::File(const StaticString& path, Allocator& allocator)
	{
		{
			std::lock_guard<std::mutex> lock(ResourceManagerGuard::instance_mutex);
			if (ResourceManagerGuard::instance)
			{
				_private = ResourceManagerGuard::instance->open_file(path)._private;
				return;
			}
		}
		_private = FilePrivate::open(path, Read, allocator);
	}
}
