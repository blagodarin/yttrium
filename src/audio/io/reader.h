#ifndef __AUDIO_IO_READER_H
#define __AUDIO_IO_READER_H

#include <Yttrium/audio/io.h>

#include "../../base/file.h"

namespace Yttrium
{

class Y_PRIVATE AudioReader::Private
	: public PrivateBase<AudioReader::Private>
{
public:

	Private(Allocator *allocator)
		: PrivateBase(allocator)
		, _offset(0)
		, _size(0)
		, _atom_size(0)
	{
	}

	virtual ~Private()
	{
	}

public:

	virtual bool open() = 0;

	virtual size_t read(void *buffer, size_t bytes_to_read) = 0;

	virtual void seek(UOffset offset) = 0;

public:

	File        _file;
	AudioFormat _format;
	UOffset     _offset;
	UOffset     _size;
	UOffset     _atom_size;
};

} // namespace Yttrium

#endif // __AUDIO_IO_READER_H
