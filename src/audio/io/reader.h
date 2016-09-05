#ifndef _src_audio_io_reader_h_
#define _src_audio_io_reader_h_

#include <yttrium/audio/io.h>

#include "../../base/file.h"

namespace Yttrium
{
	class AudioReaderImpl : public AudioReader
	{
		friend AudioReader;

	public:
		AudioFormat format() const override { return _format; }
		uint64_t offset() const override { return _offset_units; }
		uint64_t size() const override { return _total_units * _format.unit_size(); }

	protected:
		virtual bool open() = 0;

	protected:
		AudioFormat _format;
		uint64_t _total_units = 0;
		uint64_t _offset_units = 0;
		File _file;

		AudioReaderImpl(const StaticString& name, Allocator& allocator) : _file(name, &allocator) {}
	};
}

#endif
