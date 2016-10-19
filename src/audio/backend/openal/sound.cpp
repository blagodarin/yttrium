#include "sound.h"

#include <yttrium/memory/buffer.h>
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
		::alDeleteSources(1, &_source);
		::alDeleteBuffers(1, &_buffer);
	}

	bool OpenAlSound::load(AudioReader& reader)
	{
		const AudioFormat& format = reader.format();
		if (format.channels != 1 || !_format.set(format))
			return false;

		::alGenBuffers(1, &_buffer);
		if (::alGetError() != AL_NO_ERROR)
			return false;

		::alGenSources(1, &_source);
		if (::alGetError() == AL_NO_ERROR)
		{
			::alSourcei(_source, AL_SOURCE_RELATIVE, AL_TRUE);

			const auto reader_size = reader.size();
			if (reader_size > SIZE_MAX)
				throw std::bad_alloc();
			Buffer buffer(reader_size);

			if (reader.read(buffer.data(), buffer.size()) == buffer.size())
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
}
