#ifndef _src_audio_streamer_h_
#define _src_audio_streamer_h_

#include <yttrium/memory/buffer.h>

#include <memory>

namespace Yttrium
{
	class AudioPlayerBackend;
	class MusicReaderImpl;

	class AudioStreamer
	{
	public:
		AudioStreamer(AudioPlayerBackend&, MusicReaderImpl&);
		~AudioStreamer() noexcept;

		bool play_first();
		bool play_next();

	private:
		AudioPlayerBackend& _backend;
		MusicReaderImpl& _music;
		Buffer _buffer;
	};
}

#endif
