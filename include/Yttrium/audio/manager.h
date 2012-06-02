/// \file
/// \brief Audio player.

#ifndef __Y_AUDIO_MANAGER_H
#define __Y_AUDIO_MANAGER_H

#include <set>

#include <Yttrium/audio/player.h>
#include <Yttrium/noncopyable.h>

namespace Yttrium
{

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

	AudioManager(Allocator *allocator = DefaultAllocator) noexcept
		: _allocator(allocator)
		, _private(nullptr)
	{
	}

	///

	~AudioManager() noexcept
	{
		close();
	}

	///

	void close() noexcept;

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

	Allocator *_allocator;
	Private   *_private;
};

} // namespace Yttrium

#endif // __Y_AUDIO_MANAGER_H
