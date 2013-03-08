#ifndef __AUDIO_IO_WAV_H
#define __AUDIO_IO_WAV_H

#include "reader.h"

namespace Yttrium
{

struct WavChunkHeader;

class WavReader: public AudioReader::Private
{
public:

	WavReader(Allocator *allocator)
		: AudioReader::Private(allocator)
	{
	}

public: // AudioReader::Private

	bool open() override;

	size_t read(void *buffer, size_t bytes_to_read) override;

	void seek(UOffset offset) override;

private:

	bool find_chunk(uint32_t fourcc, WavChunkHeader *header);

private:

	UOffset _data_offset;
};

} // namespace Yttrium

#endif // __AUDIO_IO_WAV_H
