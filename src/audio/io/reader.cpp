#include "reader.hpp"

#include "../../file_system/file_system.hpp"

#include "ogg_vorbis.hpp"
#include "wav.hpp"

namespace Yttrium
{

AudioReader::AudioReader(const AudioReader &reader)
	: _private(Private::copy(reader._private))
{
}

void AudioReader::close()
{
	if (Private::should_free(&_private))
	{
		Private::free(&_private);
	}
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

		_private = allocator->new_<WavReader>(allocator);
		break;

	case AudioType::OggVorbis:

		_private = allocator->new_<OggVorbisReader>(allocator);
		break;

	default:

		{
			StaticString extension = name.file_extension();
			if (extension == ".wav")
			{
				_private = allocator->new_<WavReader>(allocator);
			}
			else if (extension == ".ogg")
			{
				_private = allocator->new_<OggVorbisReader>(allocator);
			}
		}
		break;
	}

	if (_private && _private->open(name, *_file_system))
	{
		return true;
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

} // namespace Yttrium
