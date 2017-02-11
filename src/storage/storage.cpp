#include <yttrium/storage/storage.h>

#include <yttrium/exceptions.h>
#include <yttrium/memory/buffer.h>
#include "reader.h"

#include <vector>

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
		std::unique_ptr<PackageReader> package;
		std::string buffer_name;
		std::shared_ptr<const Buffer> buffer;

		StorageAttachment(std::unique_ptr<PackageReader>&& package)
			: type(Type::Package)
			, package(std::move(package))
		{
		}

		StorageAttachment(std::string&& name, Buffer&& buffer)
			: type(Type::Buffer)
			, buffer_name(std::move(name))
			, buffer(std::make_shared<const Buffer>(std::move(buffer)))
		{
		}
	};

	class StoragePrivate
	{
	public:
		StoragePrivate(Storage::UseFileSystem use_file_system)
			: _use_file_system(use_file_system)
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
					if (StaticString{ attachment.buffer_name } == name)
						return Reader(std::make_shared<BufferReader>(attachment.buffer, std::string{ attachment.buffer_name }));
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
		const Storage::UseFileSystem _use_file_system;
		std::vector<StorageAttachment> _attachments;

		friend Storage;
	};

	Storage::Storage(UseFileSystem use_file_system)
		: _private(std::make_unique<StoragePrivate>(use_file_system))
	{
	}

	Storage::~Storage() = default;

	void Storage::attach_buffer(const StaticString& name, Buffer&& buffer)
	{
		_private->_attachments.emplace_back(name.to_std(), std::move(buffer));
	}

	void Storage::attach_package(const StaticString& path, PackageType type)
	{
		auto package = PackageReader::create(path, type);
		if (!package)
			throw MissingDataError("Unable to open \"", path, "\"");
		_private->_attachments.emplace_back(std::move(package));
	}

	Reader Storage::open(const StaticString& name) const
	{
		return _private->open(name);
	}
}
