#include <yttrium/resource_manager.h>

#include <yttrium/log.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/std/map.h>
#include <yttrium/std/vector.h>
#include "../base/file.h"
#include "../base/instance_guard.h"

namespace Yttrium
{
	struct ResourceAttachment
	{
		enum class Type
		{
			Package,
			Buffer,
		};

		Type type;
		UniquePtr<PackageReader> package;
		String buffer_name{ &NoAllocator };
		Buffer buffer;

		ResourceAttachment(UniquePtr<PackageReader>&& package) : type(Type::Package), package(std::move(package)) {}
		ResourceAttachment(String&& name, Buffer&& buffer) : type(Type::Buffer), buffer_name(std::move(name)), buffer(std::move(buffer)) {}
	};

	using ResourceManagerGuard = InstanceGuard<ResourceManagerPrivate>;

	class ResourceManagerPrivate
	{
	public:
		ResourceManagerPrivate(ResourceManager::UseFileSystem use_file_system, Allocator& allocator)
			: _allocator(allocator)
			, _use_file_system(use_file_system)
		{
		}

		File open(const StaticString& name) const
		{
			if (_use_file_system == ResourceManager::UseFileSystem::Before)
			{
				File file(name, File::Read, _allocator);
				if (file)
					return file;
			}
			// TODO: Build a single name-to-resource map for the entire resource system.
			for (const auto& attachment : _attachments)
			{
				if (attachment.type == ResourceAttachment::Type::Package)
				{
					auto file = attachment.package->open_file(name);
					if (file)
						return file;
				}
				else if (attachment.type == ResourceAttachment::Type::Buffer)
				{
					if (attachment.buffer_name == name)
						return File(Buffer(attachment.buffer.size(), attachment.buffer.data()), _allocator); // TODO: Share buffer contents.
				}
			}
			if (_use_file_system == ResourceManager::UseFileSystem::After)
			{
				File file(name, File::Read, _allocator);
				if (file)
					return file;
			}
			return {};
		}

	private:
		Allocator& _allocator;
		const ResourceManager::UseFileSystem _use_file_system;
		StdVector<ResourceAttachment> _attachments{ _allocator };
		ResourceManagerGuard _instance_guard{ this, "Duplicate ResourceManager construction" };

		friend ResourceManager;
	};

	ResourceManager::ResourceManager(UseFileSystem use_file_system, Allocator& allocator)
		: _private(make_unique<ResourceManagerPrivate>(allocator, use_file_system, allocator))
	{
	}

	void ResourceManager::attach_buffer(const StaticString& name, Buffer&& buffer)
	{
		_private->_attachments.emplace_back(String(name, &_private->_allocator), std::move(buffer));
	}

	bool ResourceManager::attach_package(const StaticString& path, PackageType type)
	{
		auto package = PackageReader::create(path, type, _private->_allocator);
		if (!package)
		{
			Log() << "("_s << path << ") Unable to open"_s;
			return false;
		}
		_private->_attachments.emplace_back(std::move(package));
		return true;
	}

	File ResourceManager::open(const StaticString& path)
	{
		return _private->open(path);
	}

	ResourceManager::~ResourceManager() = default;

	File::File(const StaticString& path, Allocator& allocator)
	{
		{
			std::lock_guard<std::mutex> lock(ResourceManagerGuard::instance_mutex);
			if (ResourceManagerGuard::instance)
			{
				_private = ResourceManagerGuard::instance->open(path)._private;
				return;
			}
		}
		_private = FilePrivate::open(path, Read, allocator);
	}
}
