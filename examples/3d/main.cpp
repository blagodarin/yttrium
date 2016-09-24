#include "game.h"

#include <yttrium/log.h>

#include <stdexcept>

int main()
{
	Log::set_file("3d.log");

	try
	{
		Game game;
		game.run();
		return 0;
	}
	catch (const std::runtime_error& e)
	{
		Log() << "Runtime error: " << e.what();
		return 1;
	}
}
