/// \file
/// \brief

#ifndef _include_yttrium_io_resource_loader_h_
#define _include_yttrium_io_resource_loader_h_

#include <yttrium/global.h>

#include <memory>

namespace Yttrium
{
	class AudioManager;
	template <typename> class SharedPtr;
	class Sound;
	class StaticString;
	class Storage;

	///
	class Y_API ResourceLoader
	{
	public:
		///
		ResourceLoader(const Storage&);

		///
		SharedPtr<Sound> load_sound(const StaticString& name);

		///
		void set_audio_manager(AudioManager&);

		///
		const Storage& storage() const;

	private:
		const std::unique_ptr<class ResourceLoaderPrivate> _private;

	public:
		~ResourceLoader();
	};
}

#endif
