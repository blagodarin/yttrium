#include "manager.h"

#include <yttrium/io/reader.h>
#include <yttrium/io/resource_manager.h>
#include <yttrium/log.h>
#include "sound.h"

#include "backend/openal/backend.h"

#include <cassert>

namespace Yttrium
{
	StdVector<StaticString> AudioManager::backends(Allocator& allocator)
	{
		StdVector<StaticString> result(allocator);
		result.emplace_back(AudioBackend::OpenAL);
		return result;
	}

	StdVector<StaticString> AudioManager::backend_devices(const StaticString& backend, Allocator& allocator)
	{
		if (backend == AudioBackend::OpenAL)
			return OpenAlBackend::devices(allocator);
		else
			return StdVector<StaticString>(allocator);
	}

	UniquePtr<AudioManager> AudioManager::create(const ResourceManager& resource_manager, const StaticString& backend, const StaticString& device, Allocator& allocator)
	{
		try
		{
			return make_unique<AudioManagerImpl>(allocator, resource_manager, backend, device, allocator);
		}
		catch (const AudioBackend::UnableToCreate& e)
		{
			Log() << e.what();
			return {};
		}
	}

	AudioManagerImpl* AudioManagerImpl::instance()
	{
		return AudioManagerGuard::instance;
	}

	AudioManagerImpl::AudioManagerImpl(const ResourceManager& resource_manager, const StaticString& backend, const StaticString& device, Allocator& allocator)
		: _resource_manager(resource_manager)
		, _allocator(allocator)
		, _backend(AudioBackend::create(backend, device, _allocator))
		, _player(_resource_manager, _backend->create_player(), _allocator)
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

	SharedPtr<Sound> AudioManagerImpl::create_sound(const StaticString& name)
	{
		std::lock_guard<std::mutex> lock(AudioManagerGuard::instance_mutex);

		const auto i = _sounds.find(String(name, ByReference()));
		if (i != _sounds.end())
			return SharedPtr<Sound>(*i->second.first, i->second.second);

		const auto reader = AudioReader::open(_resource_manager.open(name), AudioType::Auto, _allocator);
		if (!reader)
			return {};

		auto sound = _backend->create_sound(name, _allocator);
		if (!sound->load(*reader))
			return {};

		_sounds.emplace(sound->name(), std::make_pair(&_allocator, sound.get()));
		return std::move(sound);
	}

	void AudioManagerImpl::delete_sound(const String& name)
	{
		std::lock_guard<std::mutex> lock(AudioManagerGuard::instance_mutex);
		_sounds.erase(name);
	}
}
