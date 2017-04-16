#include "backend.h"

#include <yttrium/exceptions.h>
#include "player.h"
#include "sound.h"

namespace
{
	auto create_alc_device()
	{
		Yttrium::P_ALCdevice device{::alcOpenDevice(nullptr)};
		if (!device)
			throw Yttrium::InitializationError{"Failed to open the default OpenAL device"};
		return device;
	}

	auto create_alc_context(::ALCdevice* device)
	{
		Yttrium::P_ALCcontext context{::alcCreateContext(device, nullptr)};
		if (!context)
			throw Yttrium::InitializationError{"Failed to create an OpenAL context for \"", ::alcGetString(device, ALC_DEVICE_SPECIFIER), "\" device"};
		return context;
	}
}

namespace Yttrium
{
	OpenALBackend::OpenALBackend()
		: _device{::create_alc_device()}
		, _context{::create_alc_context(_device.get())}
	{
		::alcMakeContextCurrent(_context.get());
	}

	OpenALBackend::~OpenALBackend()
	{
		if (_context.get() == ::alcGetCurrentContext())
			::alcMakeContextCurrent(nullptr);
	}

	std::unique_ptr<AudioPlayerBackend> OpenALBackend::create_player()
	{
		return std::make_unique<OpenALPlayer>();
	}

	std::unique_ptr<Sound> OpenALBackend::create_sound(AudioReader& reader)
	{
		return std::make_unique<OpenALSound>(reader);
	}

	std::unique_ptr<AudioBackend> AudioBackend::create()
	{
		return std::make_unique<OpenALBackend>();
	}
}
