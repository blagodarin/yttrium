#include "sound.h"

#include "manager.h"

namespace Yttrium
{

SoundPtr Sound::open(const StaticString &name, Allocator *allocator)
{
	AudioManager::Private *audio_manager = AudioManager::Private::instance();
	Y_ASSERT(audio_manager);

	if (Y_LIKELY(!allocator))
		allocator = audio_manager->_allocator;

	// Lock mutex here for the sound opening to become thread safe.

	AudioManager::Private::Sounds::iterator i = audio_manager->_sounds.find(String(name, ByReference()));
	if (i != audio_manager->_sounds.end())
		return SoundPtr(i->second);

	AudioReader reader;
	if (Y_UNLIKELY(!reader.open(name, AudioType::Auto, allocator)))
		return SoundPtr();

	SoundImpl *sound = audio_manager->create_sound(name, allocator);
	if (Y_UNLIKELY(!sound->load(&reader)))
	{
		// Unlock the mutex here for the sound opening to become thread safe.
		Y_DELETE(allocator, sound);
		return SoundPtr();
	}

	audio_manager->_sounds.insert(AudioManager::Private::Sounds::value_type(sound->_name, sound));

	return SoundPtr(sound);
}

} // namespace Yttrium
