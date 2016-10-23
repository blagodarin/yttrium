#include "reader.h"

#include <yttrium/memory/unique_ptr.h>
#include <yttrium/static_string.h>
#include "../utils/fourcc.h"
#include "formats/wav.h"

#ifndef Y_NO_OGG_VORBIS
	#include "formats/ogg_vorbis.h"
#endif

#include <new>

namespace
{
	using namespace Yttrium;

	bool detect_audio_type(const Reader& reader, AudioType& type)
	{
		uint32_t signature = 0;
		if (!reader.read_at(0, signature))
			return false;
		switch (signature)
		{
		case "RIFF"_fourcc:
			type = AudioType::Wav;
			return true;
#ifndef Y_NO_OGG_VORBIS
		case "OggS"_fourcc:
			type = AudioType::OggVorbis;
			return true;
#endif
		default:
			return false;
		}
	}
}

namespace Yttrium
{
	UniquePtr<AudioReader> AudioReader::open(Reader&& reader, AudioType type, Allocator& allocator)
	{
		if (!reader)
			return {};

		if (type == AudioType::Auto && !::detect_audio_type(reader, type))
			return {};

		reader.seek(0);

		UniquePtr<AudioReaderImpl> audio_reader;
		switch (type)
		{
		case AudioType::Wav: audio_reader = make_unique<WavReader>(allocator, std::move(reader)); break;
	#ifndef Y_NO_OGG_VORBIS
		case AudioType::OggVorbis: audio_reader = make_unique<OggVorbisReader>(allocator, std::move(reader)); break;
	#endif
		default: return {};
		}

		if (!audio_reader->open())
			return {};

		return std::move(audio_reader);
	}
}
