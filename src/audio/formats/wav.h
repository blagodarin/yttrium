#ifndef _src_audio_formats_wav_h_
#define _src_audio_formats_wav_h_

#include "../reader.h"

namespace Yttrium
{
	struct WavChunkHeader;

	class WavReader : public AudioReaderImpl
	{
	public:
		WavReader(Reader&&);

		bool open() override;
		size_t read(void* buffer, size_t bytes_to_read) override;
		bool seek(uint64_t offset) override;

	private:
		bool find_chunk(uint32_t fourcc, WavChunkHeader&);

	private:
		uint64_t _data_offset = 0;
	};
}

#endif
