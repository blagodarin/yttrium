#include <yttrium/io/storage.h>

#include <yttrium/log.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/memory/unique_ptr.h>
#include <yttrium/std/map.h>
#include <yttrium/std/vector.h>
#include "reader.h"

namespace Yttrium
{
	struct StorageAttachment
	{
		enum class Type
		{
			Package,
			Buffer,
		};

		Type type;
		UniquePtr<PackageReader> package;
		String buffer_name{ &NoAllocator };
		std::shared_ptr<const Buffer> buffer;

		StorageAttachment(UniquePtr<PackageReader>&& package)
			: type(Type::Package)
			, package(std::move(package))
		{
		}

		StorageAttachment(String&& name, Buffer&& buffer)
			: type(Type::Buffer)
			, buffer_name(std::move(name))
			, buffer(std::make_shared<const Buffer>(std::move(buffer)))
		{
		}
	};

	class StoragePrivate
	{
	public:
		StoragePrivate(Storage::UseFileSystem use_file_system, Allocator& allocator)
			: _allocator(allocator)
			, _use_file_system(use_file_system)
		{
		}

		Reader open(const StaticString& name) const
		{
			if (_use_file_system == Storage::UseFileSystem::Before)
			{
				Reader reader(name);
				if (reader)
					return reader;
			}
			// TODO: Build a single name-to-resource map for the entire resource system.
			for (const auto& attachment : _attachments)
			{
				if (attachment.type == StorageAttachment::Type::Package)
				{
					auto reader = attachment.package->open(name);
					if (reader)
						return reader;
				}
				else if (attachment.type == StorageAttachment::Type::Buffer)
				{
					if (attachment.buffer_name == name)
						return Reader(std::make_shared<BufferReader>(attachment.buffer, attachment.buffer_name));
				}
			}
			if (_use_file_system == Storage::UseFileSystem::After)
			{
				Reader reader(name);
				if (reader)
					return reader;
			}
			return {};
		}

	private:
		Allocator& _allocator;
		const Storage::UseFileSystem _use_file_system;
		StdVector<StorageAttachment> _attachments{ _allocator };

		friend Storage;
	};

	Storage::Storage(UseFileSystem use_file_system, Allocator& allocator)
		: _private(std::make_unique<StoragePrivate>(use_file_system, allocator))
	{
	}

	void Storage::attach_buffer(const StaticString& name, Buffer&& buffer)
	{
		_private->_attachments.emplace_back(String(name, &_private->_allocator), std::move(buffer));
	}

	bool Storage::attach_package(const StaticString& path, PackageType type)
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

	Reader Storage::open(const StaticString& name) const
	{
		return _private->open(name);
	}

	Storage::~Storage() = default;
}
