#include "wav.h"

#include "wav_private.h"

#include <algorithm>

namespace Yttrium
{

WavReader::WavReader(const StaticString& name, Allocator* allocator)
	: AudioReaderImpl(name, allocator)
	, _data_offset(0)
{
}

bool WavReader::open()
{
	WavFileHeader  file_header;
	WavChunkHeader chunk_header;
	WavFormatChunk format_chunk;

	if (!_file.read(&file_header)
		|| file_header.riff_fourcc != WavFileHeader::RIFF
		|| file_header.wave_fourcc != WavFileHeader::WAVE)
		return false;

	if (!find_chunk(WavChunkHeader::fmt, &chunk_header))
		return false;

	if (!_file.read(&format_chunk)
		|| format_chunk.format != WAVE_FORMAT_PCM)
		return false;

	if (chunk_header.size > sizeof format_chunk
		&& !_file.skip(chunk_header.size - sizeof format_chunk))
		return false;

	if (!find_chunk(WavChunkHeader::data, &chunk_header))
		return false;

	_format.bytes_per_sample = format_chunk.bits_per_sample / 8;
	_format.channels = format_chunk.channels;
	_format.frequency = format_chunk.samples_per_second;
	_total_units = std::min<uint64_t>(_file.size() - _file.offset(), chunk_header.size) / _format.unit_size();

	_data_offset = _file.offset();

	return true;
}

size_t WavReader::read(void* buffer, size_t bytes_to_read)
{
	const size_t unit_size = _format.unit_size();
	bytes_to_read = std::min<uint64_t>(bytes_to_read / unit_size, _total_units - _offset_units) * unit_size;
	const size_t bytes_read = _file.read(buffer, bytes_to_read);
	_offset_units += bytes_read / unit_size;
	return bytes_read;
}

bool WavReader::seek(uint64_t offset_units)
{
	if (offset_units > _total_units)
		return false;
	if (!_file.seek(_data_offset + offset_units * _format.unit_size()))
		return false;
	_offset_units = offset_units;
	return true;
}

bool WavReader::find_chunk(uint32_t fourcc, WavChunkHeader* header)
{
	for (; ; )
	{
		if (!_file.read(header))
			return false;
		else if (header->name_fourcc == fourcc)
			break;
		else if (!_file.skip(header->size))
			return false;
	}
	return true;
}

} // namespace Yttrium
