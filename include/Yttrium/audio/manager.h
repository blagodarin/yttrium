/// \file
/// \brief Audio player.

#ifndef __Y_AUDIO_MANAGER_H
#define __Y_AUDIO_MANAGER_H

#include <Yttrium/audio/player.h>
#include <Yttrium/noncopyable.h>

#include <set>

namespace Yttrium
{

class ProxyAllocator;

/// Audio manager.

class Y_API AudioManager: public Noncopyable
{
public:

	///

	typedef std::set<StaticString> Backends;

	///

	typedef std::set<StaticString> Devices;

public:

	///

	AudioManager(Allocator *allocator = DefaultAllocator) noexcept;

	///

	~AudioManager() noexcept;

public:

	/// Get the current backend name.
	/// \return Audio backend name.
	/// \note The result is valid through all the library lifetime.

	StaticString backend() const noexcept;

	///

	void close() noexcept;

	/// Get the current backend device name.
	/// \return Audio output device name.
	/// \note The result is valid until the manager is closed.

	StaticString device() const noexcept;

	///

	bool open(const StaticString &backend = StaticString(), const StaticString &device = StaticString()) noexcept;

	///

	AudioPlayer player();

public:

	///

	static Backends backends() noexcept;

	///

	static Devices backend_devices(const StaticString &backend) noexcept;

public:

	class Private;

private:

	ProxyAllocator *_allocator;
	Private        *_private;
};

} // namespace Yttrium

#endif // __Y_AUDIO_MANAGER_H
