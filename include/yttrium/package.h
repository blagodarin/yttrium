/// \file
/// \brief

#ifndef _include_yttrium_package_h_
#define _include_yttrium_package_h_

#include <yttrium/memory/global.h>
#include <yttrium/memory/pointer.h>

namespace Yttrium
{
	class File;
	class StaticString;

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
		static Pointer<PackageReader> create(const StaticString& package, PackageType type = PackageType::Auto, Allocator& allocator = *DefaultAllocator);

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
		static Pointer<PackageWriter> create(const StaticString& package, PackageType type = PackageType::Auto, Allocator& allocator = *DefaultAllocator);

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
		static Pointer<PackageManager> create(Order order, Allocator& allocator = *DefaultAllocator);

		PackageManager() = default;
		virtual ~PackageManager() = default;

		/// Mount the specified package into the file system.
		/// \param name The package name.
		/// \param type Package file type.
		/// \return \c true on success.
		virtual bool mount(const StaticString& name, PackageType type = PackageType::Auto) = 0;
	};
}

#endif
