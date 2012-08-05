#include <Yttrium/yttrium.h>

using namespace Yttrium;

int main()
{
	MemoryManager memory_manager;

	LogManager log_manager(Y_S("audio.log"));

	AudioManager audio;

	audio.open();

	AudioPlayer player = audio.player();

	player.load(Y_S("example.ogg"));
	player.play();
	Thread::sleep(2000);
	player.stop();

	Thread::sleep(1000);

	player.clear();
	player.load(Y_S("example.wav"));
	player.play();
	Thread::sleep(2000);
	player.stop();

	return 0;
}
