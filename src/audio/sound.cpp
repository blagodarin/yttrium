#include "sound.h"

#include "manager.h"

namespace Yttrium
{

Sound::Sound(const Sound &sound)
	: _private(Private::copy(sound._private))
{
}

void Sound::close()
{
	if (Y_LIKELY(_private))
	{
		// NOTE: Lock mutex here for the sound closing to become thread safe.

		if (Private::release(_private))
		{
			AudioManager::Private::instance()->_sounds.erase(_private->name());
		}

		// NOTE: Unlock the thread safety mutex here.

		_private = nullptr;
	}
}

bool Sound::open(const StaticString &name, Allocator *allocator)
{
	close();

	AudioManager::Private *audio_manager = AudioManager::Private::instance();

	Y_ASSERT(audio_manager);

	if (Y_LIKELY(!allocator))
	{
		allocator = audio_manager->_allocator;
	}

	// NOTE: Lock mutex here for the sound opening to become thread safe.

	AudioManager::Private::Sounds::iterator i = audio_manager->_sounds.find(String(name, ByReference()));

	if (i != audio_manager->_sounds.end())
	{
		_private = Private::copy(i->second);
		return true;
	}

	AudioReader reader;

	if (Y_UNLIKELY(!reader.open(name, AudioType::Auto, allocator)))
	{
		return false;
	}

	_private = audio_manager->create_sound(name, allocator);

	if (Y_UNLIKELY(!_private->load(&reader)))
	{
		// NOTE: Unlock the thread safety mutex here.

		close();
		return false;
	}

	audio_manager->_sounds.insert(AudioManager::Private::Sounds::value_type(_private->name(), _private));

	return true;
}

void Sound::play() const
{
	if (Y_LIKELY(_private))
	{
		_private->play();
	}
}

Sound &Sound::operator =(const Sound &sound)
{
	Private::assign(&_private, sound._private);

	return *this;
}

} // namespace Yttrium
