#ifndef __AUDIO_BACKEND_MANAGER_H
#define __AUDIO_BACKEND_MANAGER_H

#include <yttrium/audio/manager.h>

namespace Yttrium
{

AudioManager::Private *open_audio_manager(const StaticString &backend, const StaticString &device, Allocator *allocator);

} // namespace Yttrium

#endif // __AUDIO_BACKEND_MANAGER_H
