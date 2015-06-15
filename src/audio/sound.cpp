#include "sound.h"

#include "manager.h"

namespace Yttrium
{
	SharedPtr<Sound> Sound::create(const StaticString& name, Allocator* allocator)
	{
		return AudioManagerImpl::instance()->create_sound(name, allocator);
	}
}
