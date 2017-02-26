#include <memory>

namespace Yttrium
{
	class AudioBackend;
	class AudioPlayerBackend;

	class AudioManagerPrivate
	{
	public:
		AudioManagerPrivate();

		AudioBackend& backend() const { return *_backend; }

	public:
		const std::unique_ptr<AudioBackend> _backend;
	};
}
