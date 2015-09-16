#ifndef __AUDIO_BACKEND_OPENAL_BACKEND_H
#define __AUDIO_BACKEND_OPENAL_BACKEND_H

#include "../../../base/utils.h"
#include "../../backend.h"
#include "player.h"

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
		OpenAlPlayer _player;
	};
}

#endif