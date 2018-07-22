#include "reader.h"

#include <yttrium/exceptions.h>
#include "../utils/fourcc.h"
#include "formats/wav.h"

#if Y_USE_OGG_VORBIS
	#include "formats/ogg_vorbis.h"
#endif

namespace
{
	enum class AudioType
	{
		Unknown,
		Wav,
#if Y_USE_OGG_VORBIS
		OggVorbis,
#endif
	};

	AudioType detect_audio_type(const Yttrium::Source& source)
	{
		using namespace Yttrium::Literals;

		uint32_t signature = 0;
		if (!source.read_at(0, signature))
			return AudioType::Unknown;
		switch (signature)
		{
		case "RIFF"_fourcc: return AudioType::Wav;
#if Y_USE_OGG_VORBIS
		case "OggS"_fourcc: return AudioType::OggVorbis;
#endif
		default: return AudioType::Unknown;
		}
	}
}

namespace Yttrium
{
	std::unique_ptr<AudioReader> AudioReader::open(std::unique_ptr<Source>&& source)
	{
		if (!source)
			return {};
		switch (::detect_audio_type(*source))
		{
		case AudioType::Wav: return std::make_unique<WavReader>(std::move(source));
#if Y_USE_OGG_VORBIS
		case AudioType::OggVorbis: return std::make_unique<OggVorbisReader>(std::move(source));
#endif
		default: throw DataError{"Unknown audio format"};
		}
	}
}
