/// \file
/// \brief

#ifndef _include_yttrium_package_h_
#define _include_yttrium_package_h_

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

	/// Package manager.
	class Y_API PackageManager
	{
	public:

		/// File search order.
		enum class Order
		{
			PackedFirst, ///< Try packed, then system file.
			SystemFirst, ///< Try system, then packed file.
			PackedOnly,  ///< Try packed file only.
		};

		///
		static UniquePtr<PackageManager> create(Order, Allocator& = *DefaultAllocator);

		PackageManager() = default;
		virtual ~PackageManager() = default;

		///
		virtual bool bind(const StaticString& path, Buffer&&) = 0;

		/// Mount the specified package into the file system.
		/// \param name The package name.
		/// \param type Package file type.
		/// \return \c true on success.
		virtual bool mount(const StaticString& name, PackageType = PackageType::Auto) = 0;
	};
}

#endif
