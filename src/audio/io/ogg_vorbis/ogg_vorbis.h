#ifndef __AUDIO_IO_OGG_VORBIS_H
#define __AUDIO_IO_OGG_VORBIS_H

#include "../reader.h"

#include <vorbis/vorbisfile.h>

namespace Yttrium
{

class OggVorbisReader: public AudioReaderImpl
{
public:

	OggVorbisReader(const StaticString& name, Allocator* allocator);
	~OggVorbisReader() override;

	bool open() override;
	size_t read(void* buffer, size_t bytes_to_read) override;
	bool seek(uint64_t offset) override;

private:

	OggVorbis_File _ov_file;
};

} // namespace Yttrium

#endif // __AUDIO_IO_OGG_VORBIS_H
