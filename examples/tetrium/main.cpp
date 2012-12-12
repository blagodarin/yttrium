#include <Yttrium/log.h>
#include <Yttrium/memory_manager.h>
#include <Yttrium/script/manager.h>

#include "game.h"

int main(int, char **)
{
	MemoryManager memory_manager;

	LogManager log_manager("tetrium.log");

	ScriptManager script_manager;

	Game game;

	if (game.setup())
		game.run();
	return 0;
}
