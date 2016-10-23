#ifndef _src_audio_formats_ogg_vorbis_h_
#define _src_audio_formats_ogg_vorbis_h_

#include "../reader.h"

#include <vorbis/vorbisfile.h>

namespace Yttrium
{
	class OggVorbisReader : public AudioReaderImpl
	{
	public:
		OggVorbisReader(Reader&&);
		~OggVorbisReader() override;

		bool open() override;
		size_t read(void* buffer, size_t bytes_to_read) override;
		bool seek(uint64_t offset) override;

	private:
		OggVorbis_File _ov_file;
	};
}

#endif
