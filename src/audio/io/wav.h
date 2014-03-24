#ifndef __AUDIO_IO_WAV_H
#define __AUDIO_IO_WAV_H

#include "reader.h"

namespace Yttrium
{

struct WavChunkHeader;

class WavReader: public AudioReaderImpl
{
public:

	WavReader(Allocator* allocator);

	bool open() override;
	size_t read(void* buffer, size_t bytes_to_read) noexcept override;
	bool seek(UOffset offset) noexcept override;

private:

	bool find_chunk(uint32_t fourcc, WavChunkHeader* header);

private:

	UOffset _data_offset;
};

} // namespace Yttrium

#endif // __AUDIO_IO_WAV_H
