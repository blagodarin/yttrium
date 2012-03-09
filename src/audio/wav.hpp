#ifndef __AUDIO_WAV_HPP
#define __AUDIO_WAV_HPP

#include "reader.hpp"

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

	virtual bool open(const StaticString &name, FileSystem &file_system);

	virtual size_t read(void *buffer, size_t bytes_to_read);

	virtual void seek(UOffset offset);

private:

	bool find_chunk(uint32_t fourcc, WavChunkHeader *header);

private:

	UOffset _data_offset;
};

} // namespace Yttrium

#endif // __AUDIO_WAV_HPP
