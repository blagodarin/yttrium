#include "manager.h"

#include <yttrium/allocator.h>
#include <yttrium/script/context.h>

#include "sound.h"

namespace Yttrium
{

OpenAlManager::OpenAlManager(ALCdevice *device, ALCcontext *context, const StaticString &device_name, Allocator *allocator)
	: AudioManager::Private(allocator)
	, _device(device)
	, _context(context)
{
	_device_name = device_name;
}

OpenAlManager::~OpenAlManager()
{
	if (_context == ::alcGetCurrentContext())
	{
		::alcMakeContextCurrent(nullptr);
	}

	::alcDestroyContext(_context);
	::alcCloseDevice(_device);
}

SoundImpl *OpenAlManager::create_sound(const StaticString &name, Allocator *allocator)
{
	return Y_NEW(allocator, OpenAlSound)(name, allocator);
}

AudioManager::Devices OpenAlManager::devices()
{
	AudioManager::Devices result;

	const ALCchar *devices = ::alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

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
	ALCdevice *alc_device = nullptr;

	if (!device.is_empty())
	{
		alc_device = ::alcOpenDevice(device.zero_terminated(allocator).const_text());
	}

	if (!alc_device)
	{
		ScriptValue *stored_device = ScriptContext::global().find("audio_device");
		if (stored_device)
			alc_device = ::alcOpenDevice(stored_device->string().const_text());
	}

	if (!alc_device)
	{
		alc_device = ::alcOpenDevice(nullptr);
	}

	if (alc_device)
	{
		StaticString final_device(::alcGetString(alc_device, ALC_DEVICE_SPECIFIER));
		ScriptContext::global().set("audio_device", final_device, ScriptValue::Archived);

		ALCcontext *alc_context = ::alcCreateContext(alc_device, nullptr);
		if (alc_context)
		{
			::alcMakeContextCurrent(alc_context);
			return Y_NEW(allocator, OpenAlManager)(alc_device, alc_context, final_device, allocator);
			//::alcDestroyContext(alc_context);
		}

		::alcCloseDevice(alc_device);
	}

	return nullptr;
}

} // namespace Yttrium
