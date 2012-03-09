#include "manager.hpp"

// TODO: Use script variables.

namespace Yttrium
{

AudioManager::~AudioManager()
{
	if (_device)
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(_context);
		alcCloseDevice(_device);
	}
}

bool AudioManager::initialize(const StaticString &device)
{
	if (_device)
	{
		return false;
	}

	String audio_device(device, _allocator); // Avoid non-zero-terminated problems.

	// First of all, try to open the device passed as a parameter.

	if (audio_device)
	{
		_device = alcOpenDevice(audio_device.text());
		if (_device)
		{
			audio_device.set(alcGetString(nullptr, ALC_DEVICE_SPECIFIER));
			//SCRIPT_MANAGER.set("audio_device", audio_device, true);
		}
	}

	// If no device was specified as a parameter or the specified device failed
	// to open, try to open the device specified in the "audio_device" variable.

//	if (!_device)
//	{
//		audio_device = SCRIPT_MANAGER.get("audio_device");
//		if (audio_device)
//		{
//			_device = alcOpenDevice(audio_device.text());
//		}
//	}

	// Finally, if nothing is specified or the specified device could be opened,
	// try to open the default device.

	if (!_device)
	{
		_device = alcOpenDevice(nullptr);
		if (_device)
		{
			audio_device.set(alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER));
			//SCRIPT_MANAGER.set("audio_device", audio_device, true);
		}
	}

	// If the device was successfully opened, perform the initialization.

	if (_device)
	{
		_context = alcCreateContext(_device, nullptr);
		if (_context)
		{
			Y_LOG(_logger, Y_S("Audio device opened: \"") << audio_device << '"');

			alcMakeContextCurrent(_context);
			return true;
			//alcDestroyContext(_context);
		}
		alcCloseDevice(_device);
		_device = nullptr;
	}

	return false;
}

} // namespace Yttrium
