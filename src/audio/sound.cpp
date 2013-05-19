#include "sound.h"

namespace Yttrium
{

Sound::Sound(const Sound &sound)
	: _private(Private::copy(sound._private))
{
}

Sound::~Sound()
{
	Private::release(&_private);
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
