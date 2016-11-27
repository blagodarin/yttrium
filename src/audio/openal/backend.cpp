#include "backend.h"

#include <yttrium/exceptions.h>
#include <yttrium/resources/resource_ptr.h>
#include "player.h"
#include "sound.h"

namespace
{
	using namespace Yttrium;

	P_ALCdevice create_alc_device()
	{
		P_ALCdevice device(::alcOpenDevice(nullptr));
		if (!device)
			throw InitializationError("Failed to open the default OpenAL device");
		return device;
	}

	P_ALCcontext create_alc_context(::ALCdevice* device)
	{
		P_ALCcontext context(::alcCreateContext(device, nullptr));
		if (!context)
			throw InitializationError("Failed to create an OpenAL context for \"", ::alcGetString(device, ALC_DEVICE_SPECIFIER), "\" device");
		return context;
	}
}

namespace Yttrium
{
	OpenAlBackend::OpenAlBackend()
		: _device(::create_alc_device())
		, _context(::create_alc_context(_device.get()))
	{
		::alcMakeContextCurrent(_context.get());
	}

	OpenAlBackend::~OpenAlBackend()
	{
		if (_context.get() == ::alcGetCurrentContext())
			::alcMakeContextCurrent(nullptr);
	}

	std::unique_ptr<AudioPlayerBackend> OpenAlBackend::create_player()
	{
		return std::make_unique<OpenAlPlayer>();
	}

	ResourcePtr<Sound> OpenAlBackend::create_sound(AudioReader& reader)
	{
		return make_resource<Sound_OpenAL>(reader);
	}

	std::unique_ptr<AudioBackend> AudioBackend::create()
	{
		return std::make_unique<OpenAlBackend>();
	}
}
