/// \file
/// \brief Audio player.

#ifndef __Y_AUDIO_MANAGER_H
#define __Y_AUDIO_MANAGER_H

#include <yttrium/audio/player.h>

#include <vector>

namespace Yttrium
{

class ProxyAllocator;

/// Audio manager.
class Y_API AudioManager
{
	Y_NONCOPYABLE(AudioManager);

public:

	///
	AudioManager(Allocator* allocator = DefaultAllocator);

	///
	~AudioManager();

public:

	/// Get the current backend name.
	/// \return Audio backend name.
	/// \note The result is valid through all the library lifetime.
	StaticString backend() const;

	///
	void close();

	/// Get the current backend device name.
	/// \return Audio output device name.
	/// \note The result is valid until the manager is closed.
	StaticString device() const;

	///
	bool open(const StaticString& backend = StaticString(), const StaticString& device = StaticString());

	///
	AudioPlayer player();

public:

	///
	static std::vector<StaticString> backends();

	///
	static std::vector<StaticString> backend_devices(const StaticString& backend);

public:

	class Private;

private:

	ProxyAllocator* _allocator;
	Private*        _private;
};

} // namespace Yttrium

#endif // __Y_AUDIO_MANAGER_H
