#include <Yttrium/yttrium.hpp>

using namespace Yttrium;

namespace Foo
{

void bar()
{
	Logger logger("hello_world.bar.ling");

	Y_INFO(logger, "Hello again!");
	Y_DEBUG(logger, "Hello again!");
	Y_FATAL(logger, "Hello again!");

	Logger logger2("hello_world.blurp");

	Y_INFO(logger2, "Hello again!");
	Y_DEBUG(logger2, "Hello again!");
	Y_FATAL(logger2, "Hello again!");

	Y_ASSERT(false);
}

} // namespace Foo

int main(int argc, char **argv)
{
	Application application;

	// Set up the logging.

	LogManager log_manager = application.log_manager();

	log_manager.open("hello_world.log", Logger::Rewrite, Logger::All);

	log_manager.set_level("hello_universe", Logger::Fatal);
	log_manager.set_level("hello_world", Logger::Debug);
	log_manager.set_level("hello_world.bar", Logger::Info);
	log_manager.set_level("hello_world.pwn", Logger::Fatal);

	// Initialize the application.

	application.initialize();

	// Play!

	Logger root_logger;

	Y_LOG(root_logger, "*** Yttrium example ***");

	Y_TRACE(root_logger, "Hello world?");
	Y_DEBUG(root_logger, "Hello world?");

	Logger logger("hello_world");

	Y_INFO(logger, "Hello world!");
	Y_DEBUG(logger, "Hello world!");
	Y_TRACE(logger, "Hello world!");
	Y_ERROR(logger, "Hello world!");

	Foo::bar();

	return 0;
}
