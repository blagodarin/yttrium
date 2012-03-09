#include "wav.hpp"

#include <Yttrium/file_system.hpp>

#include "../../base/fourcc.hpp"

namespace Yttrium
{

enum
{
	WAVEFORM_PCM = 1,
};

#pragma pack(push, 1)

struct WavFileHeader
{
	uint32_t riff_fourcc;
	uint32_t riff_size;
	uint32_t wave_fourcc;
};

struct WavChunkHeader
{
	uint32_t name_fourcc;
	uint32_t size;
};

struct WavFormatChunk
{
	uint16_t format;
	uint16_t channels;
	uint32_t samples_per_second;
	uint32_t bytes_per_second;
	uint16_t block_align;
	uint16_t bits_per_sample;
};

#pragma pack(pop)

namespace
{

const uint32_t FourccRiff   = Fourcc<'R', 'I', 'F', 'F'>::value;
const uint32_t FourccWave   = Fourcc<'W', 'A', 'V', 'E'>::value;
const uint32_t FourccFormat = Fourcc<'f', 'm', 't', ' '>::value;
const uint32_t FourccData   = Fourcc<'d', 'a', 't', 'a'>::value;

} // namespace

bool WavReader::open(const StaticString &name, FileSystem &file_system)
{
	WavFileHeader  file_header;
	WavChunkHeader chunk_header;
	WavFormatChunk format_chunk;
	AudioFormat    format;

	_file = file_system.open_file(name);

	if (!_file)
	{
		return false;
	}

	if (!_file.read(&file_header)
		|| file_header.riff_fourcc != FourccRiff
		|| file_header.wave_fourcc != FourccWave)
	{
		return false;
	}

	if (!find_chunk(FourccFormat, &chunk_header))
	{
		return false;
	}

	if (!_file.read(&format_chunk)
		|| format_chunk.format != WAVEFORM_PCM)
	{
		return false;
	}

	if (chunk_header.size > sizeof(format_chunk)
		&& !_file.skip(chunk_header.size - sizeof(format_chunk)))
	{
		return false;
	}

	if (!find_chunk(FourccData, &chunk_header))
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
