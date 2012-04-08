#include <Yttrium/yttrium.hpp>

using namespace Yttrium;

int main(int argc, char **argv)
{
	Application application;

	LogManager log_manager = application.log_manager();

	log_manager.open(Y_S("audio.log"), Logger::Rewrite, Logger::All);

	AudioManager audio;

	audio.open();

	return 0;
}
