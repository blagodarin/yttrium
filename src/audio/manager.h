#include <memory>

namespace Yttrium
{
	class Allocator;
	class AudioBackend;
	class AudioPlayerBackend;

	class AudioManagerPrivate
	{
	public:
		AudioManagerPrivate(Allocator&);

		std::unique_ptr<AudioPlayerBackend> create_player_backend();

	public:
		Allocator& _allocator;
		const std::unique_ptr<AudioBackend> _backend;
	};
}
