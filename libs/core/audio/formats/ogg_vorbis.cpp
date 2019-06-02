//
// Copyright 2018 Sergei Blagodarin
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

#include "ogg_vorbis.h"

#include <yttrium/exceptions.h>
#include <yttrium/utils/numeric.h>

#include <algorithm>
#include <cstring>

namespace
{
	size_t read_callback(void* ptr, size_t size, size_t nmemb, void* datasource)
	{
		return static_cast<Yttrium::Reader*>(datasource)->read(ptr, size * nmemb);
	}

	int seek_callback(void* datasource, ogg_int64_t offset, int whence)
	{
		auto& reader = *static_cast<Yttrium::Reader*>(datasource);
		switch (whence)
		{
		case SEEK_SET: return offset >= 0 && reader.seek(Yttrium::to_unsigned(offset)) ? 0 : -1;
		case SEEK_CUR: return offset >= 0 && reader.skip(Yttrium::to_unsigned(offset)) ? 0 : -1;
		case SEEK_END: return offset == 0 && reader.seek(reader.size()) ? 0 : -1;
		default: return -1;
		}
	}

	int close_callback(void*)
	{
		return 0;
	}

	long tell_callback(void* datasource)
	{
		return static_cast<long>(static_cast<Yttrium::Reader*>(datasource)->offset());
	}

	const ov_callbacks _ov_callbacks{
		read_callback,
		seek_callback,
		close_callback,
		tell_callback,
	};
}

namespace Yttrium
{
	OggVorbisReader::OggVorbisReader(std::unique_ptr<Source>&& source)
		: AudioReaderImpl{ std::move(source) }
	{
		::memset(&_ov_file, 0, sizeof _ov_file);
		if (::ov_open_callbacks(&_reader, &_ov_file, nullptr, 0, ::_ov_callbacks) < 0)
			throw DataError("Bad Ogg Vorbis file");

		vorbis_info* info = ::ov_info(&_ov_file, -1);
		if (info->channels <= 0 || info->rate <= 0)
			throw DataError("Bad Ogg Vorbis file");

		const auto total_samples = ::ov_pcm_total(&_ov_file, -1);
		if (total_samples < 0)
			throw DataError("Bad Ogg Vorbis file");

		_format = AudioFormat{ 2, to_unsigned(info->channels), to_unsigned(info->rate) };
		_total_samples = to_unsigned(total_samples);
	}

	OggVorbisReader::~OggVorbisReader()
	{
		::ov_clear(&_ov_file);
	}

	size_t OggVorbisReader::read(void* buffer, size_t bytes_to_read)
	{
		const auto block_size = _format.block_size();
		bytes_to_read = static_cast<size_t>(std::min<uint64_t>(bytes_to_read / block_size, _total_samples - _current_sample)) * block_size;
		size_t bytes_read = 0;
		for (int bitstream = 0; bytes_read <= bytes_to_read;)
		{
			const auto size = static_cast<int>(std::min<size_t>(bytes_to_read - bytes_read, to_unsigned(std::numeric_limits<int>::max())));
			const auto read = ::ov_read(&_ov_file, static_cast<char*>(buffer) + bytes_read, size, 0, 2, 1, &bitstream);
			if (read <= 0)
				break;
			bytes_read += to_unsigned(read);
		}
		_current_sample += bytes_read / block_size;
		return bytes_read;
	}

	bool OggVorbisReader::seek(uint64_t block_offset)
	{
		if (block_offset > _total_samples)
			return false;
		if (::ov_pcm_seek(&_ov_file, static_cast<ogg_int64_t>(block_offset)) != 0)
			return false;
		_current_sample = block_offset;
		return true;
	}
}
