#ifndef _src_audio_io_wav_h_
#define _src_audio_io_wav_h_

#include "reader.h"

namespace Yttrium
{
	struct WavChunkHeader;

	class WavReader : public AudioReaderImpl
	{
	public:

		WavReader(const StaticString& name, Allocator* allocator);

		bool open() override;
		size_t read(void* buffer, size_t bytes_to_read) override;
		bool seek(uint64_t offset) override;

	private:

		bool find_chunk(uint32_t fourcc, WavChunkHeader* header);

	private:

		uint64_t _data_offset;
	};
}

#endif
