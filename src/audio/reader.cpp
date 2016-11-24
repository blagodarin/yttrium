#include "reader.h"

#include <yttrium/exceptions.h>
#include "../utils/fourcc.h"
#include "formats/wav.h"

#include "../config.h"
#ifndef Y_NO_OGG_VORBIS
	#include "formats/ogg_vorbis.h"
#endif

namespace
{
	using namespace Yttrium;

	enum class AudioType
	{
		Unknown,
		Wav,
#ifndef Y_NO_OGG_VORBIS
		OggVorbis,
#endif
	};

	AudioType detect_audio_type(const Reader& reader)
	{
		uint32_t signature = 0;
		if (!reader.read_at(0, signature))
			return AudioType::Unknown;
		switch (signature)
		{
		case "RIFF"_fourcc: return AudioType::Wav;
#ifndef Y_NO_OGG_VORBIS
		case "OggS"_fourcc: return AudioType::OggVorbis;
#endif
		default: return AudioType::Unknown;
		}
	}
}

namespace Yttrium
{
	std::unique_ptr<AudioReader> AudioReader::open(Reader&& reader)
	{
		if (!reader)
			return {};
		switch (::detect_audio_type(reader))
		{
		case AudioType::Wav: return std::make_unique<WavReader>(std::move(reader)); break;
#ifndef Y_NO_OGG_VORBIS
		case AudioType::OggVorbis: return std::make_unique<OggVorbisReader>(std::move(reader)); break;
#endif
		default: throw DataError("Unknown audio format");
		}
	}
}
