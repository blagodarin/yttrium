#ifndef _src_audio_music_h_
#define _src_audio_music_h_

#include <yttrium/audio/music.h>

#include <memory>

namespace Yttrium
{
	class AudioReader;

	class MusicImpl final : public Music
	{
	public:
		MusicImpl(std::unique_ptr<AudioReader>&&);

		void set_settings(const Settings&) override;

		AudioReader& reader() const { return *_reader; }
		Settings settings() const { return _settings; }

	private:
		const std::unique_ptr<AudioReader> _reader;
		Settings _settings;
	};
}

#endif
