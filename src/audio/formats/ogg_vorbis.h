#ifndef _src_audio_formats_ogg_vorbis_h_
#define _src_audio_formats_ogg_vorbis_h_

#if !Y_USE_OGG_VORBIS
	#error Ogg Vorbis support is disabled.
#endif

#include "../reader.h"

#include <vorbis/vorbisfile.h>

namespace Yttrium
{
	class OggVorbisReader final : public AudioReaderImpl
	{
	public:
		OggVorbisReader(std::unique_ptr<Source>&&);
		~OggVorbisReader() override;

		size_t read(void*, size_t) override;
		bool seek(uint64_t) override;

	private:
		OggVorbis_File _ov_file;
	};
}

#endif
