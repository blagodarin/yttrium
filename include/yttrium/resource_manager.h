/// \file
/// \brief

#ifndef _include_yttrium_resource_manager_h_
#define _include_yttrium_resource_manager_h_

#include <yttrium/memory/unique_ptr.h>
#include <yttrium/package.h>

namespace Yttrium
{
	/// Resource manager.
	class Y_API ResourceManager
	{
	public:

		/// Resource search order.
		enum class SearchOrder
		{
			PackedFirst, ///< Try packed, then system file.
			SystemFirst, ///< Try system, then packed file.
			PackedOnly,  ///< Try packed file only.
		};

		///
		ResourceManager(SearchOrder, Allocator& = *DefaultAllocator);

		/// Adds a named buffer into the resource system.
		bool bind(const StaticString& path, Buffer&&);

		/// Mounts a package into the resource system.
		bool mount_package(const StaticString& path, PackageType = PackageType::Auto);

	private:
		const UniquePtr<class ResourceManagerPrivate> _private;

	public:
		~ResourceManager();
	};
}

#endif
