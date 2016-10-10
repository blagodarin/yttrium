#include <yttrium/io/package.h>
#include <yttrium/io/reader.h>
#include <yttrium/io/writer.h>
#include <yttrium/memory/unique_ptr.h>
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

		if (!package->open(entry).write_all(Reader(entry))) // TODO: Append.
		{
			// TODO: Remove the package file.
			std::cerr << "ERROR: Can't pack \"" << entry << "\" (" << argv[2] << ":" << line << ")" << std::endl;
			return 1;
		}
	}
}
