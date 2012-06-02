#include "manager.h"

// TODO: Use script variables.

namespace Yttrium
{

OpenAlManager::OpenAlManager(ALCdevice *device, ALCcontext *context, Allocator *allocator)
	: AudioManager::Private(allocator)
	, _device(device)
	, _context(context)
{
}

OpenAlManager::~OpenAlManager()
{
	if (_context == alcGetCurrentContext())
	{
		alcMakeContextCurrent(nullptr);
	}

	alcDestroyContext(_context);
	alcCloseDevice(_device);
}

AudioManager::Devices OpenAlManager::devices()
{
	AudioManager::Devices result;

	const ALCchar *devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
	if (devices)
	{
		for (const ALCchar *current = devices; *current; )
		{
			const ALCchar *next = current;
			do ++next; while (*next);
			result.insert(StaticString(current, next - current));
			current = next + 1;
		}
	}

	return result;
}

OpenAlManager *OpenAlManager::open(const StaticString &device, Allocator *allocator)
{
	String audio_device(device, allocator); // Avoid non-zero-terminated problems.

	ALCdevice *alc_device = nullptr;

	// First of all, try to open the device passed as a parameter.

	if (!audio_device.is_empty())
	{
		alc_device = alcOpenDevice(audio_device.text());
	}

	// If no device was specified as a parameter or the specified device failed
	// to open, try to open the device specified in the "audio_device" variable.

//	if (!alc_device)
//	{
//		audio_device = SCRIPT_MANAGER.get("audio_device");
//		if (audio_device)
//		{
//			alc_device = alcOpenDevice(audio_device.text());
//		}
//	}

	// Finally, if nothing is specified or the specified device could be opened,
	// try to open the default device.

	if (!alc_device)
	{
		alc_device = alcOpenDevice(nullptr);
	}

	// If the device was successfully opened, perform the initialization.

	if (alc_device)
	{
		audio_device.set(alcGetString(alc_device, ALC_DEVICE_SPECIFIER));
		//SCRIPT_MANAGER.set("audio_device", audio_device, true);

		ALCcontext *alc_context = alcCreateContext(alc_device, nullptr);

		if (alc_context)
		{
			Y_LOG(Logger(allocator), Y_S("OpenAL device opened: \"") << audio_device << '"'); // TODO: Fix it.

			alcMakeContextCurrent(alc_context);

			return new(allocator->allocate<OpenAlManager>())
				OpenAlManager(alc_device, alc_context, allocator);

			//alcDestroyContext(alc_context);
		}

		alcCloseDevice(alc_device);
	}

	return nullptr;
}

} // namespace Yttrium
