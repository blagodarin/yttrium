#ifndef _src_audio_music_h_
#define _src_audio_music_h_

#include <yttrium/audio/music.h>

#include <memory>

namespace Yttrium
{
	class AudioFormat;
	class AudioReader;

	class MusicReaderImpl final : public MusicReader
	{
	public:
		MusicReaderImpl(std::unique_ptr<AudioReader>&&);

		bool set_properties(int start_ms, int end_ms, int loop_ms) noexcept override;

		size_t buffer_size() const noexcept { return _buffer_samples * _block_size; }
		AudioFormat format() const noexcept;
		size_t read(void*);
		void seek_start();

	private:
		const std::unique_ptr<AudioReader> _reader;
		const size_t _block_size;
		const size_t _buffer_samples;
		uint64_t _start_sample = 0;
		uint64_t _end_sample = 0;
		uint64_t _loop_sample = 0;
		bool _is_looping = false;
	};
}

#endif
