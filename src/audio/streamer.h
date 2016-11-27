#ifndef _src_audio_streamer_h_
#define _src_audio_streamer_h_

#include <yttrium/audio/player.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/resources/resource_ptr.h>

#include <memory>

namespace Yttrium
{
	class Allocator;
	class AudioPlayerBackend;
	class AudioReader;
	class Reader;

	class AudioStreamer
	{
	public:
		enum FetchResult
		{
			Ok,
			NotEnoughData,
			NoMoreData,
		};

		AudioStreamer(AudioPlayerBackend&, Allocator&);
		~AudioStreamer();

		void close();
		FetchResult fetch();
		bool open(const ResourcePtr<const Music>&, AudioPlayer::Order);
		void prefetch();

	private:
		size_t read();

	private:
		Allocator& _allocator;
		AudioPlayerBackend& _backend;
		ResourcePtr<const Music> _music;
		uint64_t _start_sample;
		uint64_t _end_sample;
		bool _is_looping;
		uint64_t _loop_sample;
		Buffer _buffer;
		size_t _block_size;
		size_t _buffer_samples;
	};
}

#endif
