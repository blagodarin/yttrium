#ifndef _src_audio_backend_openal_backend_h_
#define _src_audio_backend_openal_backend_h_

#include "../../../base/utils.h"
#include "../../backend.h"
#include "openal.h"

#include <vector>

namespace Yttrium
{
	using P_ALCdevice = Y_UNIQUE_PTR(::ALCdevice, ::alcCloseDevice);
	using P_ALCcontext = Y_UNIQUE_PTR(::ALCcontext, ::alcDestroyContext);

	class OpenAlBackend : public AudioBackend
	{
	public:

		OpenAlBackend(P_ALCdevice device, P_ALCcontext context, Allocator*);
		~OpenAlBackend() override;

		Pointer<AudioPlayerBackend> create_player() override;
		UniquePtr<SoundImpl> create_sound(const StaticString& name, Allocator*) override;

		static std::vector<StaticString> devices();
		static Pointer<OpenAlBackend> create(const StaticString& device, Allocator*);

	private:

		const P_ALCdevice _device;
		const P_ALCcontext _context;
	};
}

#endif
