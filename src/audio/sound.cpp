#include "sound.h"

#include <yttrium/allocator.h>

#include "manager.h"

namespace Yttrium
{

SoundPtr Sound::open(const StaticString& name, Allocator* allocator)
{
	AudioManager::Private* audio_manager = AudioManager::Private::instance();

	if (!allocator)
		allocator = audio_manager->_allocator;

	// Lock mutex here for the sound opening to become thread safe.

	auto i = audio_manager->_sounds.find(String(name, ByReference()));
	if (i != audio_manager->_sounds.end())
		return SoundPtr(i->second);

	auto&& reader = AudioReader::open(name, AudioType::Auto, allocator);
	if (reader.is_null())
		return SoundPtr();

	SoundImpl* sound = audio_manager->create_sound(name, allocator);
	if (!sound->load(reader.pointer()))
	{
		// Unlock the mutex here for the sound opening to become thread safe.
		Y_DELETE(allocator, sound);
		return SoundPtr();
	}

	audio_manager->_sounds.emplace(sound->_name, sound);

	return SoundPtr(sound);
}

} // namespace Yttrium
