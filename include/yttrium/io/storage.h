/// \file
/// \brief

#ifndef _include_yttrium_io_storage_h_
#define _include_yttrium_io_storage_h_

#include <yttrium/io/package.h>

#include <memory>

namespace Yttrium
{
	class Buffer;
	class Reader;

	///
	class Y_API Storage
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
		Storage(UseFileSystem, Allocator& = *DefaultAllocator);

		/// Attaches a buffer to the storage.
		void attach_buffer(const StaticString& name, Buffer&&);

		/// Attaches a package to the storage.
		bool attach_package(const StaticString& path, PackageType = PackageType::Auto);

		/// Opens a resource.
		Reader open(const StaticString& name) const;

		~Storage();

	private:
		const std::unique_ptr<class StoragePrivate> _private;
	};
}

#endif
