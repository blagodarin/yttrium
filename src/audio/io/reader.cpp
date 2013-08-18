#include "reader.h"

#include <yttrium/package.h>
#include <yttrium/utils.h>

#include "ogg_vorbis.h"
#include "wav.h"

namespace Yttrium
{

AudioReader::AudioReader(const AudioReader &reader)
	: _private(Private::copy(reader._private))
{
}

void AudioReader::close()
{
	Private::release(&_private);
}

AudioFormat AudioReader::format() const
{
	return (_private ? _private->_format : AudioFormat());
}

UOffset AudioReader::offset()
{
	return (_private ? _private->_offset : 0);
}

bool AudioReader::open(const StaticString &name, AudioType type, Allocator *allocator)
{
	close();

	switch (type)
	{
	case AudioType::Wav:

		_private = Y_NEW(allocator, WavReader)(allocator);
		break;

	case AudioType::OggVorbis:

		_private = Y_NEW(allocator, OggVorbisReader)(allocator);
		break;

	default:

		{
			StaticString extension = name.file_extension();
			if (extension == ".wav")
			{
				_private = Y_NEW(allocator, WavReader)(allocator);
			}
			else if (extension == ".ogg")
			{
				_private = Y_NEW(allocator, OggVorbisReader)(allocator);
			}
		}
		break;
	}

	if (_private)
	{
		if (_private->_file.open(name, allocator)
			&& _private->open())
		{
			return true;
		}

		close();
	}

	close();

	return false;
}

size_t AudioReader::read(void *buffer, size_t size)
{
	if (!_private)
	{
		return 0;
	}

	size_t atom_size = _private->_atom_size;

	size_t atoms_to_read = min<UOffset>(size / atom_size, _private->_size - _private->_offset);

	size_t bytes_read = _private->read(buffer, atoms_to_read * atom_size);

	_private->_offset += bytes_read / atom_size;
	return bytes_read;
}

bool AudioReader::seek(UOffset offset)
{
	if (!_private || offset > _private->_size)
	{
		return false;
	}
	_private->seek(offset);
	_private->_offset = offset;
	return true;
}

UOffset AudioReader::size() const
{
	return (_private ? _private->_size : 0);
}

AudioReader &AudioReader::operator =(const AudioReader &reader)
{
	Private::assign(&_private, reader._private);

	return *this;
}

} // namespace Yttrium
