#include "sound.h"

#include <Yttrium/buffer.h>

namespace Yttrium
{

OpenAlSound::OpenAlSound(const StaticString &name, Allocator *allocator)
	: Private(name, allocator)
	, _source(0)
	, _buffer(0)
{
}

OpenAlSound::~OpenAlSound()
{
	::alDeleteSources(1, &_source);
	::alDeleteBuffers(1, &_buffer);
}

bool OpenAlSound::load(AudioReader *reader)
{
	Y_ASSERT(reader->is_opened());

	const AudioFormat &format = reader->format();

	if (Y_UNLIKELY(format.channels != 1 || !_format.set(format)))
	{
		return false;
	}
	
	::alGenBuffers(1, &_buffer);
	if (Y_UNLIKELY(::alGetError() != AL_NO_ERROR))
	{
		return false;
	}

	::alGenSources(1, &_source);
	if (Y_LIKELY(::alGetError() == AL_NO_ERROR))
	{
		::alSourcei(_source, AL_SOURCE_RELATIVE, AL_TRUE);

		Buffer buffer(reader->size(), _allocator); // NOTE: This might be troublesome with 4+ GB files.

		if (Y_LIKELY(reader->read(buffer.data(), buffer.size()) == buffer.size()))
		{
			::alBufferData(_buffer, _format._format, buffer.data(), buffer.size(), _format._frequency);
			if (Y_LIKELY(::alGetError() == AL_NO_ERROR))
			{
				::alSourcei(_source, AL_BUFFER, _buffer); // This must be done after alBufferData.
				return true;
			}
		}

		::alDeleteSources(1, &_source);
		_source = 0;
	}

	::alDeleteBuffers(1, &_buffer);
	_buffer = 0;

	return false;
}

void OpenAlSound::play()
{
	::alSourceStop(_source);
	::alSourcePlay(_source);
}

} // namespace Yttrium
