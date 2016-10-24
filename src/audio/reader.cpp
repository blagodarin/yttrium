#include "reader.h"

#include <yttrium/exceptions.h>
#include "../utils/fourcc.h"
#include "formats/wav.h"

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
		OggVorbis,
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
		std::unique_ptr<AudioReaderImpl> audio_reader;
		switch (::detect_audio_type(reader))
		{
		case AudioType::Wav: audio_reader = std::make_unique<WavReader>(std::move(reader)); break;
	#ifndef Y_NO_OGG_VORBIS
		case AudioType::OggVorbis: audio_reader = std::make_unique<OggVorbisReader>(std::move(reader)); break;
	#endif
		default: throw DataError("Unknown audio format");
		}
		if (!audio_reader->open())
			throw DataError("Bad audio data");
		return std::move(audio_reader);
	}
}
