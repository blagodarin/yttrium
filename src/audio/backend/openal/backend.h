#ifndef _src_audio_backend_openal_backend_h_
#define _src_audio_backend_openal_backend_h_

#include <yttrium/std/vector.h>
#include "../../../utils/unique_ptr.h"
#include "../../backend.h"
#include "openal.h"

namespace Yttrium
{
	using P_ALCdevice = Y_UNIQUE_PTR(::ALCdevice, ::alcCloseDevice);
	using P_ALCcontext = Y_UNIQUE_PTR(::ALCcontext, ::alcDestroyContext);

	class OpenAlBackend : public AudioBackend
	{
	public:
		OpenAlBackend(Allocator&);
		~OpenAlBackend() override;

		std::unique_ptr<AudioPlayerBackend> create_player() override;
		ResourcePtr<SoundImpl> create_sound() override;

	private:
		const P_ALCdevice _device;
		const P_ALCcontext _context;
	};
}

#endif
