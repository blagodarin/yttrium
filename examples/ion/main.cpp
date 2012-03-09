#include <Yttrium/yttrium.hpp>

using namespace Yttrium;

int main(int argc, char **argv)
{
	Application application;

	LogManager log_manager = application.log_manager();

	log_manager.open(Y_S("ion.log"), Logger::Rewrite, Logger::All);
	application.initialize();

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
