#include <yttrium/memory_manager.h>
#include <yttrium/package.h>
#include <yttrium/string.h>

#include <iostream>

using namespace Yttrium;

const auto index_file_extension = ".index"_s;

int main(int argc, char** argv)
{
	MemoryManager memory_manager;

	if (argc < 2 || argc > 3)
	{
		std::cerr << "Usage: ypq [-a] INDEXFILE" << std::endl;
		return 1;
	}

	PackageWriter::Mode mode = PackageWriter::Rewrite;

	if (argc > 2)
	{
		StaticString options(argv[1]);

		if (options != "-a")
		{
			std::cerr << "ERROR: Unknown options \"" << options << "\"" << std::endl;
			return 1;
		}

		mode = PackageWriter::Append;
	}

	const StaticString index_file_name = argv[argc - 1];
	if (!index_file_name.ends_with(index_file_extension))
	{
		std::cerr << "ERROR: Bad index file name \"" << index_file_name << "\"" << std::endl;
		std::cerr << "Index file names should end with \"" << index_file_extension << "\"" << std::endl;
		return 1;
	}

	File index_file(index_file_name);
	if (!index_file)
	{
		std::cerr << "ERROR: Can't open index file \"" << index_file_name << "\"" << std::endl;
		return 1;
	}

	const String package_file_name(index_file_name.left(index_file_name.size() - index_file_extension.size()));

	PackageWriter package_file(package_file_name, PackageType::Ypq, mode);
	if (!package_file)
	{
		std::cerr << "ERROR: Can't open package file \"" << package_file_name << "\"" << std::endl;
		return 1;
	}

	std::cout << "Packing files from \"" << index_file_name << "\" into \"" << package_file_name << "\"..." << std::endl;

	String line;
	for (size_t line_index = 1; index_file.read_line(line); ++line_index)
	{
		line.trim();
		if (line.is_empty() || line[0] == '#')
			continue;

		const size_t separator_index = line.find_first('=');
		if (separator_index == String::End)
		{
			std::cerr << "WARNING: Unrecognized line skipped (" << index_file_name << ":" << line_index << ")" << std::endl;
			continue;
		}

		const String source_name(line.left(separator_index).trimmed()); // Force zero-terminatedness.
		const auto& packed_name = line.right(line.size() - separator_index - 1).trimmed();
		if (source_name.is_empty() || packed_name.is_empty())
		{
			std::cerr << "WARNING: File with an empty name skipped (" << index_file_name << ":" << line_index << ")" << std::endl;
			continue;
		}

		std::cout << "Packing \"" << source_name << "\" to \"" << packed_name << "\"..." << std::endl;

		File source_file(source_name);
		File packed_file = package_file.open_file(packed_name);

		FileTransfer<8192>(&packed_file, &source_file);
	}

	return 0;
}
