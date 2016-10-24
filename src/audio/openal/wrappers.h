#ifndef _src_audio_openal_wrappers_h_
#define _src_audio_openal_wrappers_h_

#include <yttrium/exceptions.h>
#include "openal.h"

#include <array>

namespace Yttrium
{
	template <size_t N>
	class OpenALBuffers
	{
	public:
		OpenALBuffers()
		{
			::alGenBuffers(N, _buffers.data());
			if (::alGetError() != AL_NO_ERROR)
				throw InitializationError("Failed to allocate ", N, " OpenAL buffers");
		}

		~OpenALBuffers()
		{
			::alDeleteBuffers(N, _buffers.data());
		}

		ALuint& operator[](size_t index) noexcept { return _buffers[index]; }
		ALuint operator[](size_t index) const noexcept { return _buffers[index]; }

		OpenALBuffers(const OpenALBuffers&) = delete;
		OpenALBuffers& operator=(const OpenALBuffers&) = delete;

	private:
		std::array<ALuint, N> _buffers;
	};

	class OpenALBuffer
	{
	public:
		ALuint get() const noexcept { return _buffers[0]; }
	private:
		const OpenALBuffers<1> _buffers;
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

		ALuint get() const noexcept
		{
			return _source;
		}

		ALint get_int(ALenum name) const
		{
			ALint value = 0;
			::alGetSourcei(_source, name, &value); // TODO: Check result.
			return value;
		}

		void set_int(ALenum name, ALint value) const
		{
			::alSourcei(_source, name, value); // TODO: Check result.
		}

		OpenALSource(const OpenALSource&) = delete;
		OpenALSource& operator=(const OpenALSource&) = delete;

	private:
		ALuint _source = 0;
	};
}

#endif
