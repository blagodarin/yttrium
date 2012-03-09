#ifndef __AUDIO_IO_READER_HPP
#define __AUDIO_IO_READER_HPP

#include <Yttrium/audio/io.hpp>

#include "../../base/file.hpp"

namespace Yttrium
{

class FileSystem;

class AudioReader::Private: public PrivateBase<AudioReader::Private>
{
public:

	Private(Allocator *allocator)
		: PrivateBase(allocator)
	{
	}

	virtual ~Private()
	{
	}

public:

	virtual bool open(const StaticString &name, FileSystem &fileSystem) = 0;

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

#endif // __AUDIO_IO_READER_HPP
