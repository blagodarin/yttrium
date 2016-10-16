#include <yttrium/io/resource_loader.h>

#include <yttrium/audio/manager.h>
#include <yttrium/audio/sound.h>

namespace Yttrium
{
	// TODO: Resource cache.

	class ResourceLoaderPrivate
	{
	public:
		ResourceLoaderPrivate(const Storage& storage)
			: _storage(storage)
		{
		}

	public:
		const Storage& _storage;
		AudioManager* _audio_manager = nullptr;
	};

	ResourceLoader::ResourceLoader(const Storage& storage)
		: _private(std::make_unique<ResourceLoaderPrivate>(storage))
	{
	}

	SharedPtr<Sound> ResourceLoader::load_sound(const StaticString& name)
	{
		return _private->_audio_manager ? _private->_audio_manager->create_sound(name) : SharedPtr<Sound>();
	}

	void ResourceLoader::set_audio_manager(AudioManager& audio_manager)
	{
		_private->_audio_manager = &audio_manager;
	}

	const Storage& ResourceLoader::storage() const
	{
		return _private->_storage;
	}

	ResourceLoader::~ResourceLoader() = default;
}
