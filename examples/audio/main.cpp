#include <Yttrium/yttrium.hpp>

using namespace Yttrium;

int main(int argc, char **argv)
{
	Application application;

	LogManager log_manager = application.log_manager();

	log_manager.open(Y_S("audio.log"), Logger::Rewrite, Logger::All);

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
