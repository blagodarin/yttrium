#ifndef _src_audio_backend_openal_player_h_
#define _src_audio_backend_openal_player_h_

#include "../../backend.h"
#include "format.h"
#include "openal.h"

namespace Yttrium
{
	class OpenAlPlayer : public AudioPlayerBackend
	{
	public:

		OpenAlPlayer();
		~OpenAlPlayer() override;

	private:

		bool set_format(const AudioFormat& format) override;
		void fill_buffer(size_t index, void* data, size_t size) override;
		size_t check_buffers() override;
		void refill_buffer(void* data, size_t size) override;
		void play() override;
		void pause() override;
		void stop() override;

	private:

		ALuint _source = 0;
		ALuint _buffers[NumBuffers];
		OpenAlFormat _format;
	};
}

#endif
