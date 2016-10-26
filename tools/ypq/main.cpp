#include <yttrium/memory/unique_ptr.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>
#include <yttrium/string.h>

#include <iostream>

using namespace Yttrium;

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ypq PACKAGE LISTFILE" << std::endl;
		return 1;
	}

	Reader index_file(argv[2]);
	if (!index_file)
	{
		std::cerr << "ERROR: Can't open index file \"" << argv[2] << "\"" << std::endl;
		return 1;
	}

	const auto package = PackageWriter::create(argv[1], PackageType::Ypq);
	if (!package)
	{
		std::cerr << "ERROR: Can't open package file \"" << argv[1] << "\"" << std::endl;
		return 1;
	}

	String entry;
	for (size_t line = 1; index_file.read_line(entry); ++line)
	{
		entry.trim();
		if (entry.is_empty() || entry[0] == '#')
			continue;

		if (!package->add(entry, Reader(entry)))
		{
			package->unlink();
			std::cerr << "ERROR: Can't pack \"" << entry << "\" (" << argv[2] << ":" << line << ")" << std::endl;
			return 1;
		}
	}
}
