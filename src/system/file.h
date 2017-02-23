#ifndef _src_system_file_h_
#define _src_system_file_h_

#include <memory>
#include <string>

namespace Yttrium
{
	class ReaderPrivate;
	class TemporaryFile;
	class WriterPrivate;

	std::shared_ptr<ReaderPrivate> create_file_reader(const std::string&);
	std::shared_ptr<ReaderPrivate> create_file_reader(const TemporaryFile&);

	std::unique_ptr<WriterPrivate> create_file_writer(const std::string&);
	std::unique_ptr<WriterPrivate> create_file_writer(TemporaryFile&);
}

#endif
