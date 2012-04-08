#ifndef __AUDIO_OPENAL_MANAGER_HPP
#define __AUDIO_OPENAL_MANAGER_HPP

#include "../manager.hpp"

#include "player.hpp"

namespace Yttrium
{

class OpenAlManager: public AudioManager::Private
{
public:

	OpenAlManager(ALCdevice *device, ALCcontext *context, Allocator *allocator);

	virtual ~OpenAlManager();

public:

	static AudioManager::Devices devices();

	static OpenAlManager *open(const StaticString &device, Allocator *allocator);

private:

	ALCdevice    *_device;
	ALCcontext   *_context;
	OpenAlPlayer  _player;
};

} // namespace Yttrium

#endif // __AUDIO_OPENAL_MANAGER_HPP
