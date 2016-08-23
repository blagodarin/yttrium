#include "reader.h"

#include <yttrium/memory/unique_ptr.h>
#include "wav.h"

#ifndef Y_NO_OGG_VORBIS
	#include "ogg_vorbis/ogg_vorbis.h"
#endif

namespace Yttrium
{
	UniquePtr<AudioReader> AudioReader::open(const StaticString& name, AudioType type, Allocator* allocator)
	{
		if (type == AudioType::Auto)
		{
			const StaticString extension = name.file_name_extension();
			if (extension == ".wav"_s)
				type = AudioType::Wav;
#ifndef Y_NO_OGG_VORBIS
			else if (extension == ".ogg"_s)
				type = AudioType::OggVorbis;
#endif
			else
				return {};
		}

		if (!allocator)
			allocator = DefaultAllocator;

		UniquePtr<AudioReaderImpl> reader;
		switch (type)
		{
		case AudioType::Wav: reader = make_unique<WavReader>(*allocator, name, allocator); break;
#ifndef Y_NO_OGG_VORBIS
		case AudioType::OggVorbis: reader = make_unique<OggVorbisReader>(*allocator, name, allocator); break;
#endif
		default: return {};
		}

		if (!reader->_file || !reader->open())
			return {};

		return std::move(reader);
	}
}
