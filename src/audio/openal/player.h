#ifndef _src_audio_openal_player_h_
#define _src_audio_openal_player_h_

#include "../backend.h"
#include "format.h"
#include "wrappers.h"

namespace Yttrium
{
	class OpenAlPlayer : public AudioPlayerBackend
	{
	public:
		OpenAlPlayer();

	private:
		void set_format(const AudioFormat& format) override;
		void fill_buffer(size_t index, void* data, size_t size) override;
		size_t check_buffers() override;
		void refill_buffer(void* data, size_t size) override;
		void play() override;
		void pause() override;
		void stop() override;

	private:
		OpenALSource _source;
		OpenALBuffers<2> _buffers;
		OpenALFormat _format;
	};
}

#endif
