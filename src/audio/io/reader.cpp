#include "reader.h"

#include <yttrium/memory/unique_ptr.h>
#include <yttrium/static_string.h>
#include "wav.h"

#ifndef Y_NO_OGG_VORBIS
	#include "ogg_vorbis/ogg_vorbis.h"
#endif

#include <new>

namespace Yttrium
{
	UniquePtr<AudioReader> AudioReader::open(const StaticString& name, AudioType type, Allocator& allocator)
	{
		if (type == AudioType::Auto)
		{
			if (name.ends_with(".wav"_s)) // TODO: Detect format using the read data.
				type = AudioType::Wav;
#ifndef Y_NO_OGG_VORBIS
			else if (name.ends_with(".ogg"_s))
				type = AudioType::OggVorbis;
#endif
			else
				return {};
		}

		Reader reader(name);
		if (!reader)
			return {};

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
