#include "wav.h"

#include <yttrium/exceptions.h>
#include <yttrium/utils.h>
#include "wav_private.h"

namespace
{
	using namespace Yttrium;

	bool find_chunk(Reader& reader, uint32_t fourcc, WavChunkHeader& header)
	{
		for (;;)
		{
			if (!reader.read(header))
				return false;
			else if (header.name_fourcc == fourcc)
				return true;
			else if (!reader.skip(header.size))
				return false;
		}
	}
}

namespace Yttrium
{
	WavReader::WavReader(Reader&& reader)
		: AudioReaderImpl(std::move(reader))
	{
		WavFileHeader file_header;
		if (!_reader.read(file_header)
			|| file_header.riff_fourcc != WavFileHeader::RIFF
			|| file_header.wave_fourcc != WavFileHeader::WAVE)
			throw DataError("Bad WAV header");

		WavChunkHeader fmt_header;
		WavFormatChunk fmt;
		if (!::find_chunk(_reader, WavChunkHeader::fmt, fmt_header)
			|| !_reader.read(fmt)
			|| (fmt_header.size > sizeof fmt && !_reader.skip(fmt_header.size - sizeof fmt)))
			throw DataError("Bad WAV 'fmt' chunk");

		if (fmt.format != WAVE_FORMAT_PCM)
			throw DataError("Bad WAV format (must be WAVE_FORMAT_PCM)");

		WavChunkHeader data_header;
		if (!::find_chunk(_reader, WavChunkHeader::data, data_header))
			throw DataError("Bad WAV 'data' chunk");

		_format = AudioFormat(fmt.bits_per_sample / 8, fmt.channels, fmt.samples_per_second);
		_total_units = min<uint64_t>(_reader.size() - _reader.offset(), data_header.size) / _format.unit_size();
		_data_offset = _reader.offset();
	}

	size_t WavReader::read(void* buffer, size_t bytes_to_read)
	{
		const auto unit_size = _format.unit_size();
		bytes_to_read = min<uint64_t>(bytes_to_read / unit_size, _total_units - _offset_units) * unit_size;
		const auto bytes_read = _reader.read(buffer, bytes_to_read);
		_offset_units += bytes_read / unit_size;
		return bytes_read;
	}

	bool WavReader::seek(uint64_t offset_units)
	{
		if (offset_units > _total_units)
			return false;
		if (!_reader.seek(_data_offset + offset_units * _format.unit_size()))
			return false;
		_offset_units = offset_units;
		return true;
	}
}
