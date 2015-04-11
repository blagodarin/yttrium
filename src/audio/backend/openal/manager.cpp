#include "manager.h"

#include <yttrium/allocator.h>
#include "sound.h"

namespace Yttrium
{
	OpenAlManager::OpenAlManager(ALCdevice* device, ALCcontext* context, Allocator* allocator)
		: AudioManager::Private(allocator, ::alcGetString(device, ALC_DEVICE_SPECIFIER))
		, _device(device)
		, _context(context)
	{
	}

	OpenAlManager::~OpenAlManager()
	{
		if (_context == ::alcGetCurrentContext())
			::alcMakeContextCurrent(nullptr);
		::alcDestroyContext(_context);
		::alcCloseDevice(_device);
	}

	SoundImpl* OpenAlManager::create_sound(const StaticString& name, Allocator* allocator)
	{
		return Y_NEW(allocator, OpenAlSound)(name, allocator);
	}

	std::vector<StaticString> OpenAlManager::devices()
	{
		std::vector<StaticString> result;

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

	OpenAlManager* OpenAlManager::open(const StaticString& device, Allocator* allocator)
	{
		ALCdevice* alc_device = nullptr;

		if (!device.is_empty())
		{
			Y_ZERO_TERMINATED(device_z, device);
			alc_device = ::alcOpenDevice(device_z);
		}

		if (!alc_device)
			alc_device = ::alcOpenDevice(nullptr);

		if (alc_device)
		{
			ALCcontext* alc_context = ::alcCreateContext(alc_device, nullptr);
			if (alc_context)
			{
				::alcMakeContextCurrent(alc_context);
				return Y_NEW(allocator, OpenAlManager)(alc_device, alc_context, allocator);
				//::alcDestroyContext(alc_context);
			}

			::alcCloseDevice(alc_device);
		}

		return nullptr;
	}
}
