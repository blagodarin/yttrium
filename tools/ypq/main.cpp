#include <yttrium/file.h>
#include <yttrium/package.h>
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

	File list_file(argv[2]);
	if (!list_file)
	{
		std::cerr << "ERROR: Can't open index file \"" << argv[2] << "\"" << std::endl;
		return 1;
	}

	const auto& package_file = PackageWriter::create(argv[1], PackageType::Ypq);
	if (!package_file)
	{
		std::cerr << "ERROR: Can't open package file \"" << argv[1] << "\"" << std::endl;
		return 1;
	}

	String entry;
	for (size_t line = 1; list_file.read_line(entry); ++line)
	{
		entry.trim();
		if (entry.is_empty() || entry[0] == '#')
			continue;

		std::cout << entry << std::endl;

		File source_file(entry);
		if (!source_file)
		{
			// TODO: Remove the package file.
			std::cerr << "ERROR: Can't open \"" << entry << "\" (" << argv[2] << ":" << line << ")" << std::endl;
			return 1;
		}

		File packed_file = package_file->open_file(entry);

		FileTransfer<8192>(&packed_file, &source_file);
	}

	return 0;
}
