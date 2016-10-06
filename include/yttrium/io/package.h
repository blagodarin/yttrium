/// \file
/// \brief

#ifndef _include_yttrium_io_package_h_
#define _include_yttrium_io_package_h_

#include <yttrium/memory/global.h>

namespace Yttrium
{
	class Buffer;
	class File;
	class StaticString;

	template <typename> class UniquePtr;

	/// Package file types.
	enum class PackageType
	{
		Auto, ///< Automatical detection.
		Ypq,  ///< YPQ package.
	};

	/// Package reader class.
	class Y_API PackageReader
	{
	public:
		///
		static UniquePtr<PackageReader> create(const StaticString& path, PackageType = PackageType::Auto, Allocator& = *DefaultAllocator);

		PackageReader() = default;
		virtual ~PackageReader() = default;

		///
		virtual File open_file(const StaticString& name) = 0;
	};

	/// Package writer class.
	class Y_API PackageWriter
	{
	public:
		///
		static UniquePtr<PackageWriter> create(const StaticString& path, PackageType = PackageType::Auto, Allocator& = *DefaultAllocator);

		PackageWriter() = default;
		virtual ~PackageWriter() = default;

		///
		virtual File open_file(const StaticString& name) = 0;
	};
}

#endif
