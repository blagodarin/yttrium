#include <Yttrium/yttrium.hpp>

using namespace Yttrium;

namespace Foo
{

void bar()
{
	Logger logger("hello_world.bar");

	Y_INFO(logger, "Hello again!");
}

} // namespace Foo

int main(int argc, char **argv)
{
	Application application;

	Logger::open("hello_world.log");
	Logger::set_root_level(Logger::All);

	application.initialize();

	Logger root_logger;

	Y_LOG(root_logger, "*** Yttrium example ***");

	Logger logger("hello_world");

	Y_DEBUG(logger, "Hello world!");

	Foo::bar();

	return 0;
}
