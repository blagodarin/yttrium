#ifndef __AUDIO_IO_READER_H
#define __AUDIO_IO_READER_H

#include <yttrium/audio/io.h>

#include "../../base/file.h"

namespace Yttrium
{

class AudioReaderImpl: public AudioReader
{
	friend AudioReader;

protected:

	virtual bool open() = 0;

protected:

	File _file;

	AudioReaderImpl(Allocator* allocator): AudioReader(allocator) {}
};

} // namespace Yttrium

#endif // __AUDIO_IO_READER_H
