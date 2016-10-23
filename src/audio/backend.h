#ifndef _src_audio_backend_h_
#define _src_audio_backend_h_

#include <yttrium/string.h>

#include <memory>

namespace Yttrium
{
	class AudioFormat;
	template <typename> class ResourcePtr;
	class SoundImpl;

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
		static std::unique_ptr<AudioBackend> create(Allocator&);

		virtual ~AudioBackend() = default;

		virtual std::unique_ptr<AudioPlayerBackend> create_player() = 0;
		virtual ResourcePtr<SoundImpl> create_sound() = 0;

	protected:
		AudioBackend(Allocator& allocator) : _allocator(allocator) {}

		Allocator& allocator() const { return _allocator; }

	private:
		Allocator& _allocator;
	};
}

#endif
