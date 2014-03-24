#ifndef __AUDIO_BACKEND_OPENAL_MANAGER_H
#define __AUDIO_BACKEND_OPENAL_MANAGER_H

#include "../../manager.h"
#include "player.h"

namespace Yttrium
{

class OpenAlManager: public AudioManager::Private
{
public:

	OpenAlManager(ALCdevice* device, ALCcontext* context, const StaticString& device_name, Allocator* allocator);
	~OpenAlManager() override;

public:

	SoundImpl* create_sound(const StaticString& name, Allocator* allocator) override;

public:

	static std::vector<StaticString> devices();

	static OpenAlManager* open(const StaticString &device, Allocator *allocator);

private:

	ALCdevice*   _device;
	ALCcontext*  _context;
	OpenAlPlayer _player;
};

} // namespace Yttrium

#endif // __AUDIO_BACKEND_OPENAL_MANAGER_H
