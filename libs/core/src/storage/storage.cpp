//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/storage/storage.h>

#include <yttrium/exceptions.h>
#include <yttrium/memory/buffer.h>
#include "source.h"

#include <list>
#include <map>
#include <variant>
#include <vector>

// TODO: Hash-based packed file access.

namespace Yttrium
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
		std::unique_ptr<Source> operator()(const BufferEntry& entry) const { return create_source(entry._attachment->_buffer, entry._attachment->_name); }
		std::unique_ptr<Source> operator()(const PackageEntry& entry) const { return entry._package->open(entry._index); }

	private:
		const Storage::UseFileSystem _use_file_system;
		std::vector<std::unique_ptr<const PackageReader>> _packages;
		std::list<BufferAttachment> _buffers;
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
		auto package = PackageReader::create(path, type);
		if (!package)
			throw MissingDataError{ "Unable to open \"", path.string(), "\"" };
		_private->attach_package(std::move(package));
	}

	std::unique_ptr<Source> Storage::open(std::string_view name) const
	{
		return _private->open(name);
	}
}
