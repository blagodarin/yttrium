#include "backend.h"

#include <yttrium/log.h>
#include <yttrium/memory/unique_ptr.h>
#include "../../../utils/zero_terminated.h"
#include "player.h"
#include "sound.h"

namespace Yttrium
{
	OpenAlBackend::OpenAlBackend(P_ALCdevice device, P_ALCcontext context, Allocator& allocator)
		: AudioBackend(OpenAL, ::alcGetString(device.get(), ALC_DEVICE_SPECIFIER), allocator)
		, _device(std::move(device))
		, _context(std::move(context))
	{
	}

	OpenAlBackend::~OpenAlBackend()
	{
		if (_context.get() == ::alcGetCurrentContext())
			::alcMakeContextCurrent(nullptr);
	}

	UniquePtr<AudioPlayerBackend> OpenAlBackend::create_player()
	{
		return make_unique<OpenAlPlayer>(allocator());
	}

	std::shared_ptr<SoundImpl> OpenAlBackend::create_sound(const StaticString& name, Allocator& allocator)
	{
		return std::make_shared<OpenAlSound>(name, allocator);
	}

	StdVector<StaticString> OpenAlBackend::devices(Allocator& allocator)
	{
		StdVector<StaticString> result(allocator);

		const ALCchar* devices = ::alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

		if (devices)
		{
			for (const ALCchar* current = devices; *current; )
			{
				const ALCchar* next = current;
				do ++next; while (*next);
				result.emplace_back(current, next - current);
				current = next + 1;
			}
		}

		return result;
	}

	UniquePtr<OpenAlBackend> OpenAlBackend::create(const StaticString& device, Allocator& allocator)
	{
		P_ALCdevice alc_device;

		if (!device.is_empty())
		{
			Y_ZERO_TERMINATED(device_z, device);
			alc_device.reset(::alcOpenDevice(device_z));
			if (!alc_device)
				Log() << "(audio/openal) Unable to open device \""_s << device << "\""_s;
		}

		if (!alc_device)
			alc_device.reset(::alcOpenDevice(nullptr));

		if (!alc_device)
			throw UnableToCreate(String("(audio/openal) Failed to create device"_s, &allocator));

		P_ALCcontext alc_context(::alcCreateContext(alc_device.get(), nullptr));
		if (!alc_context)
			throw UnableToCreate(String("(audio/openal) Failed to create context"_s, &allocator));

		::alcMakeContextCurrent(alc_context.get());
		return make_unique<OpenAlBackend>(allocator, std::move(alc_device), std::move(alc_context), allocator);
	}
}
