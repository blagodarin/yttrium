#ifndef _src_system_file_h_
#define _src_system_file_h_

#include <memory>

namespace Yttrium
{
	class ReaderPrivate;
	class StaticString;
	class TemporaryFile;
	class WriterPrivate;

	std::shared_ptr<ReaderPrivate> create_file_reader(const StaticString&);
	std::shared_ptr<ReaderPrivate> create_file_reader(const TemporaryFile&);

	std::unique_ptr<WriterPrivate> create_file_writer(const StaticString&);
	std::unique_ptr<WriterPrivate> create_file_writer(TemporaryFile&);
}

#endif
