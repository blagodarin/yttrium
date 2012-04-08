#ifndef __AUDIO_BACKEND_MANAGER_HPP
#define __AUDIO_BACKEND_MANAGER_HPP

#include <Yttrium/audio/manager.hpp>

namespace Yttrium
{

AudioManager::Private *open_audio_manager(const StaticString &backend, const StaticString &device, Allocator *allocator);

} // namespace Yttrium

#endif // __AUDIO_BACKEND_MANAGER_HPP
