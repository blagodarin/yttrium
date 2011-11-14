#include <Yttrium/yttrium.hpp>

using namespace Yttrium;

namespace Foo
{

void bar()
{
	Logger logger(Y_S("hello_world.bar.ling"));

	Y_LOG_INFO(logger, Y_S("Hello again!"));
	Y_LOG_DEBUG(logger, Y_S("Hello again!"));
	Y_LOG_FATAL(logger, Y_S("Hello again!"));

	Logger logger2(Y_S("hello_world.blurp"));

	Y_LOG_INFO(logger2, Y_S("Hello again!"));
	Y_LOG_DEBUG(logger2, Y_S("Hello again!"));
	Y_LOG_FATAL(logger2, Y_S("Hello again!"));

	Y_ASSERT(false);
}

} // namespace Foo

int main(int argc, char **argv)
{
	Application application;

	// Set up the logging.

	LogManager log_manager = application.log_manager();

	log_manager.open(Y_S("hello_world.log"), Logger::Rewrite, Logger::All);

	log_manager.set_level(Y_S("hello_universe"), Logger::Fatal);
	log_manager.set_level(Y_S("hello_world"), Logger::Debug);
	log_manager.set_level(Y_S("hello_world.bar"), Logger::Info);
	log_manager.set_level(Y_S("hello_world.pwn"), Logger::Fatal);

	// Initialize the application.

	application.initialize();

	// Play!

	Logger root_logger;

	Y_LOG(root_logger, Y_S("*** Yttrium example ***"));

	Y_LOG_TRACE(root_logger, Y_S("Hello world?"));
	Y_LOG_DEBUG(root_logger, Y_S("Hello world?"));

	Logger logger(Y_S("hello_world"));

	Y_LOG_INFO(logger, Y_S("Hello world!"));
	Y_LOG_DEBUG(logger, Y_S("Hello world!"));
	Y_LOG_TRACE(logger, Y_S("Hello world!"));
	Y_LOG_ERROR(logger, Y_S("Hello world!"));

	Foo::bar();

	return 0;
}
