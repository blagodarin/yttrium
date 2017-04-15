#include <yttrium/storage/storage.h>

#include <yttrium/exceptions.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/static_string.h>
#include "source.h"

#include <list>
#include <vector>

#include <boost/variant/variant.hpp>

namespace Yttrium
{
	class StoragePrivate
	{
	public:
		explicit StoragePrivate(Storage::UseFileSystem use_file_system)
			: _use_file_system{use_file_system}
		{
		}

		void attach_buffer(const std::string& name, Buffer&& buffer)
		{
			_buffers.emplace_back(name, std::move(buffer));
			_stored[StaticString{_buffers.back()._name}] = BufferEntry{&_buffers.back()};
		}

		void attach_package(std::unique_ptr<PackageReader>&& package)
		{
			_packages.emplace_back(std::move(package));
			const auto p = _packages.back().get();
			const auto& names = p->names();
			for (std::size_t i = 0; i < names.size(); ++i)
				_stored[names[i]] = PackageEntry{p, i};
		}

		std::unique_ptr<Source> open(const StaticString& name) const
		{
			if (_use_file_system == Storage::UseFileSystem::Before)
			{
				auto source = Source::from(name.to_std());
				if (source)
					return source;
			}
			{
				const auto i = _stored.find(name);
				if (i != _stored.end()) // TODO-17: Use init-statement.
					return boost::apply_visitor(*this, i->second);
			}
			if (_use_file_system == Storage::UseFileSystem::After)
			{
				auto source = Source::from(name.to_std());
				if (source)
					return source;
			}
			return {};
		}

	public:
		struct BufferAttachment
		{
			std::string _name;
			std::shared_ptr<const Buffer> _buffer;
			BufferAttachment(const std::string& name, Buffer&& buffer) : _name{name}, _buffer{std::make_shared<const Buffer>(std::move(buffer))} {}
		};

		struct BufferEntry
		{
			const BufferAttachment* _attachment;
			explicit BufferEntry(const BufferAttachment* attachment) : _attachment{attachment} {}
		};

		struct PackageEntry
		{
			const PackageReader* _package;
			std::size_t _index;
			PackageEntry(const PackageReader* package, std::size_t index) : _package{package}, _index{index} {}
		};

		std::unique_ptr<Source> operator()(const boost::blank&) const { return {}; }
		std::unique_ptr<Source> operator()(const BufferEntry& entry) const { return create_source(entry._attachment->_buffer, entry._attachment->_name); }
		std::unique_ptr<Source> operator()(const PackageEntry& entry) const { return entry._package->open(entry._index); }

	private:
		const Storage::UseFileSystem _use_file_system;
		std::vector<std::unique_ptr<const PackageReader>> _packages;
		std::list<BufferAttachment> _buffers;
		std::map<StaticString, boost::variant<boost::blank, BufferEntry, PackageEntry>> _stored;
	};

	Storage::Storage(UseFileSystem use_file_system)
		: _private{std::make_unique<StoragePrivate>(use_file_system)}
	{
	}

	Storage::~Storage() = default;

	void Storage::attach_buffer(const std::string& name, Buffer&& buffer)
	{
		_private->attach_buffer(name, std::move(buffer));
	}

	void Storage::attach_package(const std::string& path, PackageType type)
	{
		auto package = PackageReader::create(path, type);
		if (!package)
			throw MissingDataError("Unable to open \"", path, "\"");
		_private->attach_package(std::move(package));
	}

	std::unique_ptr<Source> Storage::open(const StaticString& name) const
	{
		return _private->open(name);
	}
}
