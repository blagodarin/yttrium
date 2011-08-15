#include <Yttrium/yttrium.hpp>

namespace Y = Yttrium;

int main(int argc, char **argv)
{
	Y::Logger::open("hello_world.log");

	Y::Application application;

	Y::Logger logger("hello_world");

	Y_LOG(logger, "Hello world!");

	return 0;
}
