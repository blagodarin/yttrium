#ifndef _src_platform_file_h_
#define _src_platform_file_h_

#include <memory>
#include <string>

namespace Yttrium
{
	class TemporaryFile;
	class WriterPrivate;

	std::unique_ptr<WriterPrivate> create_file_writer(const std::string&);
	std::unique_ptr<WriterPrivate> create_file_writer(TemporaryFile&);
}

#endif
