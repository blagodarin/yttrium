#ifndef __AUDIO_IO_OGG_VORBIS_H
#define __AUDIO_IO_OGG_VORBIS_H

#include "../reader.h"

#include <vorbis/vorbisfile.h>

namespace Yttrium
{

class OggVorbisReader
	: public AudioReader::Private
{
public:

	OggVorbisReader(Allocator *allocator)
		: AudioReader::Private(allocator)
	{
	}

	~OggVorbisReader() override;

public: // AudioReader::Private

	bool open() override;

	size_t read(void *buffer, size_t bytes_to_read) override;

	void seek(UOffset offset) override;

private:

	OggVorbis_File _ov_file;
};

} // namespace Yttrium

#endif // __AUDIO_IO_OGG_VORBIS_H
