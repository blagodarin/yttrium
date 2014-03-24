#include "sound.h"

#include <yttrium/buffer.h>

#include "../../manager.h"

namespace Yttrium
{

void OpenAlSound::play() const
{
	::alSourceStop(_source);
	::alSourcePlay(_source);
}

OpenAlSound::~OpenAlSound()
{
	// It is not enough to lock the erase operation for a sound deletion to become thread safe.
	// It may be possible that a thread has already initiated a destruction (i.e. has entered here)
	// while some other thread is trying to open the sound, causing the reference counter to zero-trip.
	// TODO: Implement deleters to work around this problem.

	AudioManager::Private::instance()->_sounds.erase(_name);

	::alDeleteSources(1, &_source);
	::alDeleteBuffers(1, &_buffer);
}

bool OpenAlSound::load(AudioReader* reader)
{
	const AudioFormat& format = reader->format();
	if (format.channels != 1 || !_format.set(format))
		return false;
	
	::alGenBuffers(1, &_buffer);
	if (::alGetError() != AL_NO_ERROR)
		return false;

	::alGenSources(1, &_source);
	if (::alGetError() == AL_NO_ERROR)
	{
		::alSourcei(_source, AL_SOURCE_RELATIVE, AL_TRUE);

		Buffer buffer(reader->size(), allocator()); // NOTE: This might be troublesome with 4+ GB files.

		if (reader->read(buffer.data(), buffer.size()) == buffer.size())
		{
			::alBufferData(_buffer, _format._format, buffer.data(), buffer.size(), _format._frequency);
			if (::alGetError() == AL_NO_ERROR)
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

} // namespace Yttrium
