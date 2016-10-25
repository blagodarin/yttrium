#ifndef _src_audio_openal_wrappers_h_
#define _src_audio_openal_wrappers_h_

#include <yttrium/exceptions.h>
#include "openal.h"

namespace Yttrium
{
	class OpenALBuffer
	{
	public:
		OpenALBuffer()
		{
			::alGenBuffers(1, &_buffer);
			if (::alGetError() != AL_NO_ERROR)
				throw InitializationError("Failed to allocate OpenAL buffer");
		}

		~OpenALBuffer()
		{
			::alDeleteBuffers(1, &_buffer);
		}

		operator ALuint() const { return _buffer; }
		const ALuint* operator&() const { return &_buffer; }

		OpenALBuffer(const OpenALBuffer&) = delete;
		OpenALBuffer& operator=(const OpenALBuffer&) = delete;

	private:
		ALuint _buffer = 0;
	};

	class OpenALSource
	{
	public:
		OpenALSource()
		{
			::alGenSources(1, &_source);
			if (::alGetError() != AL_NO_ERROR)
				throw InitializationError("Failed to allocate OpenAL source");
		}

		~OpenALSource()
		{
			::alDeleteSources(1, &_source);
		}

		operator ALuint() const { return _source; }

		OpenALSource(const OpenALSource&) = delete;
		OpenALSource& operator=(const OpenALSource&) = delete;

	private:
		ALuint _source = 0;
	};
}

#endif
