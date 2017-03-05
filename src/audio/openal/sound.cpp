#include "sound.h"

#include <yttrium/audio/format.h>
#include <yttrium/audio/reader.h>
#include <yttrium/memory/buffer.h>

#include <limits>
#include <new>

namespace Yttrium
{
	Sound_OpenAL::Sound_OpenAL(AudioReader& reader)
		: _format(reader.format())
	{
		if (reader.format().channels() != 1)
			throw DataError("Sound audio must have one channel");
		const auto reader_size = reader.total_bytes();
		if (reader_size > std::numeric_limits<size_t>::max())
			throw std::bad_alloc();
		Buffer buffer(reader_size);
		if (reader.read(buffer.data(), buffer.size()) != buffer.size())
			throw DataError("Bad audio data");
		::alBufferData(_buffer, _format._format, buffer.data(), buffer.size(), _format._frequency);
		if (::alGetError() != AL_NO_ERROR)
			throw std::runtime_error("Failed to initialize a Sound");
		::alSourcei(_source, AL_BUFFER, static_cast<ALint>(_buffer)); // This must be done after alBufferData.
		::alSourcei(_source, AL_SOURCE_RELATIVE, AL_TRUE);
	}

	void Sound_OpenAL::play() const
	{
		::alSourceStop(_source);
		::alSourcePlay(_source);
	}
}
