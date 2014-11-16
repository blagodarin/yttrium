#include "reader.h"

#include "wav.h"

#ifndef Y_NO_OGG_VORBIS
	#include "ogg_vorbis/ogg_vorbis.h"
#endif

namespace Yttrium
{

AudioReaderPtr AudioReader::open(const StaticString& name, AudioType type, Allocator* allocator)
{
	if (type == AudioType::Auto)
	{
		StaticString extension = name.file_extension();
		if (extension == S(".wav"))
			type = AudioType::Wav;
#ifndef Y_NO_OGG_VORBIS
		else if (extension == S(".ogg"))
			type = AudioType::OggVorbis;
#endif
		else
			return AudioReaderPtr();
	}

	AudioReaderImpl* reader = nullptr;
	switch (type)
	{
	case AudioType::Wav:       reader = Y_NEW(allocator, WavReader)(name, allocator); break;
#ifndef Y_NO_OGG_VORBIS
	case AudioType::OggVorbis: reader = Y_NEW(allocator, OggVorbisReader)(name, allocator); break;
#endif
	default:                   return AudioReaderPtr();
	}

	if (!reader->_file || !reader->open())
	{
		Y_DELETE(allocator, reader);
		return AudioReaderPtr();
	}

	return AudioReaderPtr(reader);
}

} // namespace Yttrium
