#include <Yttrium/yttrium.h>

#include <cstdio>

using namespace Yttrium;

int main(int argc, char **argv)
{
	MemoryManager memory_manager;

	if (argc < 2 || argc > 3)
	{
		printf("Usage: ypq [-a] INDEXFILE\n");
		return 1;
	}

	PackageWriter::Mode mode = PackageWriter::Rewrite;

	if (argc > 2)
	{
		StaticString options(argv[1]);

		if (options != Y_S("-a"))
		{
			printf("ypq: ERROR: Unknown options \"%s\"\n", options.text());
			return 1;
		}

		mode = PackageWriter::Append;
	}

	StaticString index_file_name = argv[argc - 1];
	StaticString index_file_extension = Y_S(".index");

	if (!index_file_name.ends_with(index_file_extension))
	{
		printf("ypq: ERROR: Bad index file name \"%s\"\n", index_file_name.text());
		printf("ypq: Index file names should end with \"%s\"\n", index_file_extension.text());
		return 1;
	}

	File index_file;

	if (!index_file.open(index_file_name))
	{
		printf("ypq: ERROR: Can't open index file \"%s\"\n", index_file_name.text());
		return 1;
	}

	String package_file_name = index_file_name.left(index_file_name.size() - index_file_extension.size());

	PackageWriter package_file;

	if (!package_file.open(package_file_name, PackageType::Ypq, mode))
	{
		printf("ypq: ERROR: Can't open package file \"%s\"\n", package_file_name.text());
		return 1;
	}

	printf("ypq: Packing files from \"%s\" into \"%s\"...\n",
		index_file_name.text(), package_file_name.text());

	String line;

	for (size_t line_index = 1; index_file.read_line(&line); ++line_index)
	{
		line.trim();

		if (line.is_empty() || line[0] == '#')
		{
			continue;
		}

		size_t separator_index = line.find_first('=');

		if (separator_index == String::End)
		{
			printf("ypq: WARNING: Unrecognized line skipped (%s:%d)\n",
				index_file_name.text(), line_index);
			continue;
		}

		String source_name = line.left(separator_index).trimmed(); // Force zero-terminatedness.
		StaticString packed_name = line.right(line.size() - separator_index - 1).trimmed();

		if (source_name.is_empty() || packed_name.is_empty())
		{
			printf("ypq: WARNING: File with an empty name skipped (%s:%d)\n",
				index_file_name.text(), line_index);
			continue;
		}

		printf("ypq: Packing \"%s\" to \"%s\"...\n", source_name.text(), packed_name.text());

		File source_file(source_name);
		File packed_file = package_file.open_file(packed_name);

		FileTransfer<8192>(&packed_file, &source_file);
	}

	return 0;
}