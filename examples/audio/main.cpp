#include <Yttrium/yttrium.h>

using namespace Yttrium;

int main()
{
	MemoryManager memory_manager;

	LogManager log_manager(S("audio.log"));

	AudioManager audio;

	audio.open();

	AudioPlayer player = audio.player();

	player.load(S("example.ogg"));
	player.play();
	Thread::sleep(2000);
	player.stop();

	Thread::sleep(1000);

	player.clear();
	player.load(S("example.wav"));
	player.play();
	Thread::sleep(2000);
	player.stop();

	return 0;
}
