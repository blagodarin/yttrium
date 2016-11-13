/// \file
/// \brief

#ifndef _include_yttrium_audio_music_h_
#define _include_yttrium_audio_music_h_

#include <yttrium/global.h>
#include <yttrium/resources/resource.h>

namespace Yttrium
{
	class Reader;
	template <typename> class ResourcePtr;

	/// Music.
	class Y_API Music : public Resource
	{
	public:
		///
		/// Returns 'nullptr' for a null Reader.
		/// May throw DataError.
		static ResourcePtr<Music> open(Reader&&);

		/// Start, end and loop (re)start time in seconds.
		/// \note No loop if \a loop >= \a end.
		struct Settings
		{
			double start = 0; ///< Position to start playback at.
			double end = 0; ///< Position to end playback at.
			double loop = 0; ///< Position to loop playback at.
		};

		///
		virtual void set_settings(const Settings&) = 0;
	};
}

#endif
