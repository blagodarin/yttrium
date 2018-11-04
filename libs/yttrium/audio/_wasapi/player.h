#pragma once

#include "../backend.h"

namespace Yttrium
{
	class WasapiAudioPlayer final : public AudioPlayerBackend
	{
	public:
		WasapiAudioPlayer();

	private:
		void set_format(const AudioFormat&) override;
		void fill_buffer(size_t index, void* data, size_t size) override;
		size_t check_buffers() override;
		void refill_buffer(void* data, size_t size) override;
		void play() override;
		void pause() override;
		void stop() noexcept override;
	};
}
