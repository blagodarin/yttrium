#include <Yttrium/yttrium.h>

using namespace Yttrium;

int main(int argc, char **argv)
{
	MemoryManager memory_manager;

	LogManager log_manager(Y_S("ion.log"));

	Ion::Document document;

	if (document.load(Y_S("example.ion")))
	{
		document.save(Y_S("example_indented.ion"));
		document.save(Y_S("example_unindented.ion"), Ion::DontIndent);

		if (document.load(Y_S("example_indented.ion")))
		{
			document.save(Y_S("example_indented2.ion"));
		}

		if (document.load(Y_S("example_unindented.ion")))
		{
			document.save(Y_S("example_unindented2.ion"), Ion::DontIndent);
		}
	}

	return 0;
}
