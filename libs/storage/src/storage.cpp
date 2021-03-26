// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/storage.h>

#include <yttrium/exceptions.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/storage/source.h>
#include "source.h"

#include <list>
#include <map>
#include <variant>
#include <vector>

#include <fmt/format.h>

// TODO: Hash-based packed file access.

namespace Yt
{
	class StoragePrivate
	{
	public:
		explicit StoragePrivate(Storage::UseFileSystem use_file_system)
			: _use_file_system{ use_file_system }
		{
		}

		void attach_buffer(std::string_view name, Buffer&& buffer)
		{
			auto& attachment = _buffers.emplace_back(name, std::move(buffer));
			_stored[attachment._name] = BufferEntry{ &attachment };
		}

		void attach_package(std::unique_ptr<PackageReader>&& package)
		{
			const auto p = _packages.emplace_back(std::move(package)).get();
			const auto& names = p->names();
			for (std::size_t i = 0; i < names.size(); ++i)
				_stored[names[i]] = PackageEntry{ p, i };
		}

		std::unique_ptr<Source> open(std::string_view name) const
		{
			if (_use_file_system == Storage::UseFileSystem::Before)
				if (auto source = Source::from(name))
					return source;

			// cppcheck-suppress stlIfFind
			if (const auto i = _stored.find(name); i != _stored.end())
				return std::visit(*this, i->second);

			if (_use_file_system == Storage::UseFileSystem::After)
				if (auto source = Source::from(name))
					return source;

			return {};
		}

	public:
		struct BufferAttachment
		{
			std::string _name;
			std::shared_ptr<const Buffer> _buffer;
			BufferAttachment(std::string_view name, Buffer&& buffer)
				: _name{ name }, _buffer{ std::make_shared<const Buffer>(std::move(buffer)) } {}
		};

		struct BufferEntry
		{
			const BufferAttachment* _attachment;
			explicit BufferEntry(const BufferAttachment* attachment)
				: _attachment{ attachment } {}
		};

		struct PackageEntry
		{
			const PackageReader* _package;
			std::size_t _index;
			PackageEntry(const PackageReader* package, std::size_t index)
				: _package{ package }, _index{ index } {}
		};

		std::unique_ptr<Source> operator()(const std::monostate&) const { return {}; }
		std::unique_ptr<Source> operator()(const BufferEntry& entry) const { return create_source(entry._attachment->_buffer); }
		std::unique_ptr<Source> operator()(const PackageEntry& entry) const { return entry._package->open(entry._index); }

	private:
		const Storage::UseFileSystem _use_file_system;
		std::vector<std::unique_ptr<const PackageReader>> _packages;
		std::list<BufferAttachment> _buffers;
		std::vector<std::unique_ptr<Source>> _sources;
		std::map<std::string_view, std::variant<std::monostate, BufferEntry, PackageEntry>> _stored;
	};

	Storage::Storage(UseFileSystem use_file_system)
		: _private{ std::make_unique<StoragePrivate>(use_file_system) }
	{
	}

	Storage::~Storage() = default;

	void Storage::attach_buffer(std::string_view name, Buffer&& buffer)
	{
		_private->attach_buffer(name, std::move(buffer));
	}

	void Storage::attach_package(const std::filesystem::path& path, PackageType type)
	{
		auto source = Source::from(path);
		if (!source)
			throw MissingDataError{ fmt::format("Missing \"{}\"", path.string()) };
		auto package = PackageReader::create(std::move(source), type);
		if (!package)
			throw DataError{ fmt::format("Unable to load package \"{}\"", path.string()) };
		_private->attach_package(std::move(package));
	}

	std::unique_ptr<Source> Storage::open(std::string_view name) const
	{
		return _private->open(name);
	}
}
