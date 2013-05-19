#include "format.h"

namespace Yttrium
{

bool OpenAlFormat::set(const AudioFormat &format)
{
	switch (format.depth)
	{
	case 1: // 8 bits per sample.

		switch (format.channels)
		{
		case 1:  _format = AL_FORMAT_MONO8;   break;
		case 2:  _format = AL_FORMAT_STEREO8; break;
		default: return false;
		}
		break;

	case 2: // 16 bits per sample.

		switch (format.channels)
		{
		case 1:  _format = AL_FORMAT_MONO16;   break;
		case 2:  _format = AL_FORMAT_STEREO16; break;
		default: return false;
		}
		break;

	default:

		return false;
	}
	_frequency = format.frequency; // TODO: Check the frequency too.
	return true;
}

} // namespace Yttrium
