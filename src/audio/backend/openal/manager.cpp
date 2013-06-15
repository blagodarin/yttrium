#include "manager.h"

#include <Yttrium/script/context.h>

#include "sound.h"

namespace Yttrium
{

OpenAlManager::OpenAlManager(ALCdevice *device, ALCcontext *context, const String &device_name, Allocator *allocator)
	: AudioManager::Private(allocator)
	, _device(device)
	, _context(context)
{
	_device_name = device_name;
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

Sound::Private *OpenAlManager::create_sound(const StaticString &name, Allocator *allocator)
{
	return Y_NEW(allocator, OpenAlSound)(name, allocator);
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
	String audio_device(device, allocator); // Avoid non-zero-terminatedness problems.

	ALCdevice *alc_device = nullptr;

	// First of all, try to open the device passed as a parameter.

	if (!audio_device.is_empty())
	{
		alc_device = alcOpenDevice(audio_device.const_text());
	}

	// If no device was specified as a parameter or the specified device failed
	// to open, try to open the device specified in the "audio_device" variable.

	if (!alc_device)
	{
		ScriptValue *audio_device = ScriptContext::global().find("audio_device");
		if (audio_device)
		{
			alc_device = alcOpenDevice(audio_device->string().const_text());
		}
	}

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
		ScriptContext::global().set("audio_device", audio_device, ScriptValue::Archived);

		ALCcontext *alc_context = alcCreateContext(alc_device, nullptr);

		if (alc_context)
		{
			alcMakeContextCurrent(alc_context);
			return Y_NEW(allocator, OpenAlManager)(alc_device, alc_context, audio_device, allocator);
			//alcDestroyContext(alc_context);
		}

		alcCloseDevice(alc_device);
	}

	return nullptr;
}

} // namespace Yttrium
