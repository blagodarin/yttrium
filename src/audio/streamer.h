#ifndef _src_audio_streamer_h_
#define _src_audio_streamer_h_

#include <yttrium/audio/io.h>
#include <yttrium/audio/player.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/memory/unique_ptr.h>

namespace Yttrium
{
	class AudioPlayerBackend;

	class AudioStreamer
	{
	public:

		enum FetchResult
		{
			Ok,
			NotEnoughData,
			NoMoreData,
		};

	public:

		AudioStreamer(AudioPlayerBackend& backend, Allocator& allocator)
			: _allocator(allocator)
			, _backend(backend)
		{
		}

		void close();

		FetchResult fetch();

		bool open(Reader&&, const AudioPlayer::Settings&, AudioType, AudioPlayer::Order);

		void prefetch();

	private:

		size_t read();

	private:

		Allocator&           _allocator;
		AudioPlayerBackend&  _backend;
		UniquePtr<AudioReader> _source;
		uint64_t             _begin_sample;
		uint64_t             _end_sample;
		bool                 _is_looping;
		uint64_t             _loop_sample;
		Buffer               _buffer;
		size_t               _unit_size;
		size_t               _buffer_units;
	};
}

#endif
