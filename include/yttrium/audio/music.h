#ifndef _include_yttrium_audio_music_h_
#define _include_yttrium_audio_music_h_

#include <yttrium/api.h>

#include <memory>

namespace Yttrium
{
	class Source;

	/// Music data with corresponding audio decoder.
	class Y_API MusicReader
	{
	public:
		///
		/// Returns 'nullptr' for a null Source pointer.
		/// May throw DataError.
		static std::unique_ptr<MusicReader> open(std::unique_ptr<Source>&&);

		virtual ~MusicReader() noexcept = default;

		///
		/// \param start_ms Offset to the start of a playback range.
		/// \param end_ms Offset to the end of a playback range, must yield at least a two-second range.
		/// \param loop_ms Offset to the start of a playback loop, must yield at least a one-second loop.
		virtual bool set_properties(int start_ms, int end_ms, int loop_ms) noexcept = 0;
	};
}

#endif
