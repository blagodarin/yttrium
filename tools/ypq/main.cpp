#include <cstdio>

#include <Yttrium/yttrium.h>

using namespace Yttrium;

int main(int argc, char **argv)
{
	MemoryManager memory_manager;

	if (argc < 2)
	{
		printf("Usage: %s INDEXFILE\n", argv[0]);
		return 1;
	}

	File index_file;

	if (!index_file.open(argv[1]))
	{
		printf("ypq: Can't open index file \"%s\"\n", argv[1]);
		return 1;
	}

	String line;

	while (index_file.read_line(&line))
	{
		printf("ypq: Packing \"%s\"...\n", line.text());
	}

	return 0;
}
