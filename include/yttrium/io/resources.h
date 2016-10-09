/// \file
/// \brief

#ifndef _include_yttrium_io_resources_h_
#define _include_yttrium_io_resources_h_

#include <yttrium/io/package.h>

#include <memory>

namespace Yttrium
{
	class Reader;

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

		/// Attaches a buffer to the resource system.
		void attach_buffer(const StaticString& name, Buffer&&);

		/// Attaches a package to the resource system.
		bool attach_package(const StaticString& path, PackageType = PackageType::Auto);

		/// Opens a resource.
		Reader open(const StaticString& name) const;

	private:
		const std::unique_ptr<class ResourceManagerPrivate> _private;

	public:
		~ResourceManager();
	};
}

#endif
