#ifndef _src_audio_openal_backend_h_
#define _src_audio_openal_backend_h_

#include "../../platform/openal.h"
#include "../../utils/unique_ptr.h"
#include "../backend.h"

namespace Yttrium
{
	using P_ALCdevice = Y_UNIQUE_PTR(::ALCdevice, ::alcCloseDevice);
	using P_ALCcontext = Y_UNIQUE_PTR(::ALCcontext, ::alcDestroyContext);

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

#endif
