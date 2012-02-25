#include <Yttrium/yttrium.hpp>

using namespace Yttrium;

int main(int argc, char **argv)
{
	Application application;

	LogManager log_manager = application.log_manager();

	log_manager.open(Y_S("ion.log"), Logger::Rewrite, Logger::All);
	application.initialize();

	Ion::Document document;

	if (document.load("examples/ion/example.ion"))
	{
		document.save("copy.ion");
		document.save("copy2.ion", Ion::DontIndent);
	}

	return 0;
}
