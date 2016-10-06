/// \file
/// \brief

#ifndef _include_yttrium_io_resources_h_
#define _include_yttrium_io_resources_h_

#include <yttrium/io/package.h>
#include <yttrium/memory/unique_ptr.h>

namespace Yttrium
{
	/// Resource manager.
	class Y_API ResourceManager
	{
	public:
		///
		enum class UseFileSystem
		{
			After,
			Before,
			Never,
		};

		///
		ResourceManager(UseFileSystem, Allocator& = *DefaultAllocator);

		///
		~ResourceManager();

		/// Attaches a buffer to the resource system.
		void attach_buffer(const StaticString& name, Buffer&&);

		/// Attaches a package to the resource system.
		bool attach_package(const StaticString& path, PackageType = PackageType::Auto);

		/// Opens a resource.
		File open(const StaticString& path) const;

	private:
		const UniquePtr<class ResourceManagerPrivate> _private;
	};
}

#endif
