#ifndef __AUDIO_AL_MANAGER_HPP
#define __AUDIO_AL_MANAGER_HPP

#include <Yttrium/allocator.hpp>
#include <Yttrium/logger.hpp>

#include "al.hpp"

namespace Yttrium
{

class AudioManager
{
public:

	AudioManager(Allocator *allocator = HeapAllocator::instance())
		: _allocator(allocator)
		, _logger(Y_S("audio.manager"), _allocator)
		, _device(nullptr)
	{
	}

	~AudioManager();

	bool initialize(const StaticString &device);

private:

	Allocator  *_allocator;
	Logger      _logger;
	ALCdevice  *_device;
	ALCcontext *_context;
};

} // namespace Yttrium

#endif // __AUDIO_AL_MANAGER_HPP
