#include "wav.h"

#include "wav_private.h"

namespace Yttrium
{

bool WavReader::open()
{
	WavFileHeader  file_header;
	WavChunkHeader chunk_header;
	WavFormatChunk format_chunk;
	AudioFormat    format;

	if (!_file.read(&file_header)
		|| file_header.riff_fourcc != WavFileHeader::RIFF
		|| file_header.wave_fourcc != WavFileHeader::WAVE)
	{
		return false;
	}

	if (!find_chunk(WavChunkHeader::fmt, &chunk_header))
	{
		return false;
	}

	if (!_file.read(&format_chunk)
		|| format_chunk.format != WAVE_FORMAT_PCM)
	{
		return false;
	}

	if (chunk_header.size > sizeof(format_chunk)
		&& !_file.skip(chunk_header.size - sizeof(format_chunk)))
	{
		return false;
	}

	if (!find_chunk(WavChunkHeader::data, &chunk_header))
	{
		return false;
	}

	_format.depth     = format_chunk.bits_per_sample / 8;
	_format.channels  = format_chunk.channels;
	_format.frequency = format_chunk.samples_per_second;

	UOffset size = _file.size() - _file.offset();
	if (size > chunk_header.size)
	{
		size = chunk_header.size;
	}

	_size = size;
	_atom_size = _format.atom_size();

	_data_offset = _file.offset();

	return true;
}

size_t WavReader::read(void *buffer, size_t bytes_to_read)
{
	return _file.read(buffer, bytes_to_read);
}

void WavReader::seek(UOffset offset)
{
	_file.seek(_data_offset + offset * _atom_size);
}

bool WavReader::find_chunk(uint32_t fourcc, WavChunkHeader *header)
{
	for (; ; )
	{
		if (!_file.read(header))
		{
			return false;
		}

		if (header->name_fourcc == fourcc)
		{
			break;
		}
		else if (!_file.skip(header->size))
		{
			return false;
		}
	}
	return true;
}

} // namespace Yttrium
