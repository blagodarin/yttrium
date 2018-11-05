#pragma once

#include "../../../core/utils/memory.h"
#include "../../application/openal.h"
#include "../backend.h"

namespace Yttrium
{
	using P_ALCdevice = UniquePtr<::ALCdevice, ::alcCloseDevice>;
	using P_ALCcontext = UniquePtr<::ALCcontext, ::alcDestroyContext>;

	class OpenALBackend final : public AudioBackend
	{
	public:
		OpenALBackend();
		~OpenALBackend() override;

		std::unique_ptr<AudioPlayerBackend> create_player() override;
		std::unique_ptr<Sound> create_sound(AudioReader&) override;

	private:
		const P_ALCdevice _device;
		const P_ALCcontext _context;
	};
}
