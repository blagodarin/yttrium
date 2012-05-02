#ifndef __AUDIO_IO_OGG_VORBIS_HPP
#define __AUDIO_IO_OGG_VORBIS_HPP

// <vorbis/vorbisfile.h> includes <stdint.h> without defining __STDC_LIMIT_MACROS,
// so we should explicitly include <stdint.h> before it.

#include <Yttrium/types.hpp>

#include <vorbis/vorbisfile.h>

#include "reader.hpp"

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

	virtual bool open(const StaticString &name, FileSystem &file_system);

	virtual size_t read(void *buffer, size_t bytes_to_read);

	virtual void seek(UOffset offset);

private:

	OggVorbis_File _ov_file;
};

} // namespace Yttrium

#endif // __AUDIO_IO_OGG_VORBIS_HPP
