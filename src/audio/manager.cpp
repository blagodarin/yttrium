#include "manager.h"

#include <yttrium/memory_manager.h>
#include "sound.h"

#include "backend/openal/backend.h"

#include <cassert>

namespace Yttrium
{
	std::vector<StaticString> AudioManager::backends()
	{
		std::vector<StaticString> result;
		result.emplace_back(AudioBackend::OpenAL);
		return result;
	}

	std::vector<StaticString> AudioManager::backend_devices(const StaticString& backend)
	{
		if (backend == AudioBackend::OpenAL)
			return OpenAlBackend::devices();
		else
			return {};
	}

	Pointer<AudioManager> AudioManager::create(const StaticString& backend, const StaticString& device, Allocator* allocator)
	{
		if (!allocator)
			allocator = MemoryManager::default_allocator();
		try
		{
			return make_pointer<AudioManagerImpl>(*allocator, backend, device, allocator);
		}
		catch (const std::runtime_error&)
		{
			// TODO: Log.
			return {};
		}
	}

	AudioManagerImpl* AudioManagerImpl::instance()
	{
		return AudioManagerGuard::instance;
	}

	AudioManagerImpl::AudioManagerImpl(const StaticString& backend, const StaticString& device, Allocator* allocator)
		: _allocator("audio"_s, allocator)
		, _backend(AudioBackend::create(backend, device, &_allocator))
		, _player(_backend->create_player(), &_allocator)
		, _instance_guard(this, "Duplicate AudioManager construction")
	{
	}

	AudioManagerImpl::~AudioManagerImpl()
	{
		assert(_sounds.empty());
	}

	StaticString AudioManagerImpl::backend() const
	{
		return _backend->backend();
	}

	StaticString AudioManagerImpl::device() const
	{
		return _backend->device();
	}

	// TODO: Lock the instance mutex outside these functions.

	SharedPtr<Sound> AudioManagerImpl::create_sound(const StaticString& name, Allocator* allocator)
	{
		if (!allocator)
			allocator = &_allocator;

		std::lock_guard<std::mutex> lock(AudioManagerGuard::instance_mutex);

		const auto i = _sounds.find(String(name, ByReference()));
		if (i != _sounds.end())
			return SharedPtr<Sound>(i->second);

		const auto reader = AudioReader::open(name, AudioType::Auto, allocator);
		if (!reader)
			return {};

		auto sound = _backend->create_sound(name, allocator);
		if (!sound->load(*reader))
			return {};

		_sounds.emplace(sound->name(), sound.get());
		return std::move(sound);
	}

	void AudioManagerImpl::delete_sound(const StaticString& name)
	{
		std::lock_guard<std::mutex> lock(AudioManagerGuard::instance_mutex);
		_sounds.erase(String(name, ByReference()));
	}
}
