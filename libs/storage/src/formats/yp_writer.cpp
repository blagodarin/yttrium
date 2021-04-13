// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/yp_writer.h>

#include <yttrium/base/buffer.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>
#include "yp_format.h"

#include <vector>

namespace Yt
{
	struct YpWriterEntry
	{
		std::string _name;
		explicit YpWriterEntry(std::string_view name)
			: _name{ name } {}
	};

	struct YpWriterData
	{
		Writer _writer;
		std::vector<YpWriterEntry> _entries;
		bool _committed = false;
		bool _finished = false;
	};

	YpWriter::YpWriter(Writer&& writer)
		: _data{ std::make_unique<YpWriterData>(std::move(writer)) }
	{
	}

	YpWriter::~YpWriter() noexcept = default;

	bool YpWriter::add(const std::string& path)
	{
		if (_data->_committed)
			return false;
		_data->_entries.emplace_back(path);
		return true;
	}

	bool YpWriter::commit()
	{
		if (std::exchange(_data->_committed, true))
			return false;

		std::vector<YpFileEntry> entries;

		const auto metadataOffset = sizeof(YpFileHeader) + sizeof(YpFileEntry) * _data->_entries.size();

		Buffer metadataBuffer;
		{
			Writer writer{ metadataBuffer };

			const auto write_string = [&writer](const std::string& value) {
				const auto size = static_cast<uint8_t>(value.size()); //-V1029
				return size == value.size() && writer.write(size) && writer.write(value.data(), size);
			};

			for (const auto& entry : _data->_entries)
			{
				entries.emplace_back()._metadataOffset = static_cast<uint32_t>(metadataOffset + writer.offset());
				if (!write_string(entry._name))
					return false;
			}
		}

		const auto dataOffset = metadataOffset + metadataBuffer.size();

		for (auto& entry : entries)
			entry._dataOffset = dataOffset;

		YpFileHeader header;
		header._signature = YpFileHeader::kSignature;
		header._entryCount = static_cast<uint32_t>(_data->_entries.size());
		header._indexSize = static_cast<uint32_t>(dataOffset);
		if (!_data->_writer.write(header))
			return false;

		if (_data->_writer.write(entries.data(), entries.size() * sizeof(YpFileEntry)) != entries.size() * sizeof(YpFileEntry))
			return false;

		if (_data->_writer.write(metadataBuffer.data(), metadataBuffer.size()) != metadataBuffer.size())
			return false;

		for (const auto& entry : _data->_entries)
		{
			const auto source = Source::from(entry._name);
			if (!source)
				return false;
			const auto i = static_cast<size_t>(&entry - _data->_entries.data());
			entries[i]._dataOffset = _data->_writer.offset();
			entries[i]._dataSize = static_cast<uint32_t>(source->size());
			if (entries[i]._dataSize != source->size())
				return false;
			if (!_data->_writer.write_all(*source))
				return false;
		}

		_data->_writer.seek(sizeof(YpFileHeader));
		if (_data->_writer.write(entries.data(), entries.size() * sizeof(YpFileEntry)) != entries.size() * sizeof(YpFileEntry))
			return false;

		_data->_finished = true;
		return true;
	}
}
