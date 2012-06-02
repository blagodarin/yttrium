#ifndef __AUDIO_IO_OGG_VORBIS_H
#define __AUDIO_IO_OGG_VORBIS_H

#define __STDC_LIMIT_MACROS

#include <vorbis/vorbisfile.h>

#include "reader.h"

namespace Yttrium
{

class OggVorbisReader: public AudioReader::Private
{
public:

	OggVorbisReader(Allocator *allocator)
		: AudioReader::Private(allocator)
	{
	}

	virtual ~OggVorbisReader();

public: // AudioReader::Private

	virtual bool open();

	virtual size_t read(void *buffer, size_t bytes_to_read);

	virtual void seek(UOffset offset);

private:

	OggVorbis_File _ov_file;
};

} // namespace Yttrium

#endif // __AUDIO_IO_OGG_VORBIS_H
