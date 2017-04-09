#ifndef _src_audio_streamer_h_
#define _src_audio_streamer_h_

#include <yttrium/memory/buffer.h>

#include <memory>

namespace Yttrium
{
	class AudioPlayerBackend;
	class Music;

	class AudioStreamer
	{
	public:
		enum FetchResult
		{
			Ok,
			NotEnoughData,
			NoMoreData,
		};

		explicit AudioStreamer(AudioPlayerBackend&);
		~AudioStreamer();

		FetchResult fetch();
		bool open(const std::shared_ptr<const Music>&);
		void start();

	private:
		size_t read();

	private:
		AudioPlayerBackend& _backend;
		std::shared_ptr<const Music> _music;
		uint64_t _start_sample = 0;
		uint64_t _end_sample = 0;
		bool _is_looping = false;
		uint64_t _loop_sample = 0;
		Buffer _buffer;
		size_t _block_size = 0;
		size_t _buffer_samples = 0;
	};
}

#endif
