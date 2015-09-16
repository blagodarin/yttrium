#ifndef __AUDIO_BACKEND_H
#define __AUDIO_BACKEND_H

#include <yttrium/string.h>

namespace Yttrium
{
	class AudioFormat;
	class SoundImpl;
	template <typename> class Pointer;
	template <typename> class UniquePtr;

	class AudioPlayerBackend
	{
	public:

		enum
		{
			NumBuffers = 2,
		};

		virtual ~AudioPlayerBackend() = default;

		virtual bool set_format(const AudioFormat& format) = 0;
		virtual void fill_buffer(size_t index, void* data, size_t size) = 0;
		virtual size_t check_buffers() = 0;
		virtual void refill_buffer(void* data, size_t size) = 0;
		virtual void play() = 0;
		virtual void pause() = 0;
		virtual void stop() = 0;
	};

	class AudioBackend
	{
	public:

		static const S OpenAL;

		static Pointer<AudioBackend> create(const StaticString& backend, const StaticString& device, Allocator*);

		virtual ~AudioBackend() = default;

		virtual Pointer<AudioPlayerBackend> create_player() = 0;
		virtual UniquePtr<SoundImpl> create_sound(const StaticString& name, Allocator*) = 0;

		StaticString backend() const { return _backend; }
		StaticString device() const { return _device; }

	protected:

		AudioBackend(const StaticString& backend, const StaticString& device, Allocator* allocator)
			: _backend(backend, allocator)
			, _device(device, allocator)
		{
		}

		Allocator& allocator() const
		{
			return *_backend.allocator();
		}

	private:

		const String _backend;
		const String _device;
	};
}

#endif