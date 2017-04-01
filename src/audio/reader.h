#ifndef _src_audio_io_reader_h_
#define _src_audio_io_reader_h_

#include <yttrium/audio/reader.h>

#include <yttrium/audio/format.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>

namespace Yttrium
{
	class AudioReaderImpl : public AudioReader
	{
		friend AudioReader;

	public:
		uint64_t current_sample() const override { return _current_sample; }
		AudioFormat format() const override { return _format; }
		uint64_t total_bytes() const override { return _total_samples * _format.block_size(); }

	protected:
		AudioFormat _format;
		uint64_t _total_samples = 0;
		uint64_t _current_sample = 0;
		const std::unique_ptr<Source> _source;
		Reader _reader{*_source};

		AudioReaderImpl(std::unique_ptr<Source>&& source)
			: _source(std::move(source))
		{
		}
	};
}

#endif
