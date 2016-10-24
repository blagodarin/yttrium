#include "sound.h"

#include <yttrium/audio/io.h>
#include <yttrium/memory/buffer.h>

#include <new>

namespace Yttrium
{
	Sound_OpenAL::Sound_OpenAL(AudioReader& reader)
		: _format(reader.format())
	{
		if (reader.format().channels != 1)
			throw DataError("Sound audio must have one channel");
		const auto reader_size = reader.size();
		if (reader_size > SIZE_MAX)
			throw std::bad_alloc();
		Buffer buffer(reader_size);
		if (reader.read(buffer.data(), buffer.size()) != buffer.size())
			throw DataError("Bad audio data");
		::alBufferData(_buffer.get(), _format._format, buffer.data(), buffer.size(), _format._frequency);
		if (::alGetError() != AL_NO_ERROR)
			throw std::runtime_error("Failed to initialize a Sound");
		_source.set_int(AL_BUFFER, _buffer.get()); // This must be done after alBufferData.
		_source.set_int(AL_SOURCE_RELATIVE, AL_TRUE);
	}

	void Sound_OpenAL::play() const
	{
		::alSourceStop(_source.get());
		::alSourcePlay(_source.get());
	}
}
