#ifndef _src_audio_streamer_h_
#define _src_audio_streamer_h_

#include <yttrium/memory/buffer.h>

#include <memory>

namespace Yttrium
{
	class AudioPlayerBackend;
	class MusicReader;
	class MusicReaderImpl;

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
		bool open(const std::shared_ptr<MusicReader>&);
		void start();

	private:
		AudioPlayerBackend& _backend;
		std::shared_ptr<MusicReaderImpl> _music;
		size_t _block_size = 0;
		size_t _buffer_samples = 0;
		Buffer _buffer;
	};
}

#endif
