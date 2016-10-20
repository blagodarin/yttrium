#include "manager.h"

#include <yttrium/io/reader.h>
#include <yttrium/io/storage.h>
#include <yttrium/log.h>
#include <yttrium/resource/resource_ptr.h>
#include "sound.h"

#include "backend/openal/backend.h"

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

	UniquePtr<AudioManager> AudioManager::create(const Storage& storage, const StaticString& backend, const StaticString& device, Allocator& allocator)
	{
		try
		{
			return make_unique<AudioManagerImpl>(allocator, storage, backend, device, allocator);
		}
		catch (const AudioBackend::UnableToCreate& e)
		{
			Log() << e.what();
			return {};
		}
	}

	AudioManagerImpl::AudioManagerImpl(const Storage& storage, const StaticString& backend, const StaticString& device, Allocator& allocator)
		: _storage(storage)
		, _allocator(allocator)
		, _backend(AudioBackend::create(backend, device, _allocator))
		, _player(_storage, _backend->create_player(), _allocator)
	{
	}

	StaticString AudioManagerImpl::backend() const
	{
		return _backend->backend();
	}

	ResourcePtr<Sound> AudioManagerImpl::create_sound(const StaticString& name)
	{
		const auto reader = AudioReader::open(_storage.open(name), AudioType::Auto, _allocator);
		if (!reader)
			return {};
		auto sound = _backend->create_sound(name, _allocator);
		if (!sound->load(*reader))
			return {};
		return sound;
	}

	StaticString AudioManagerImpl::device() const
	{
		return _backend->device();
	}
}
