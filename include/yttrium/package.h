/// \file
/// \brief

#ifndef __Y_PACKAGE_H
#define __Y_PACKAGE_H

#include <yttrium/base.h>
#include <yttrium/file.h>

namespace Yttrium
{
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
		PackageReader(const StaticString& name, PackageType type = PackageType::Auto, Allocator* allocator = DefaultAllocator);

		///
		File open_file(const StaticString& name);

	private:
		Y_SHARED_PRIVATE(PackageReader);
	};

	/// Package writer class.
	class Y_API PackageWriter
	{
	public:

		/// Package writing mode.
		enum Mode
		{
			Rewrite, ///< Replace the destination file with the package.
			Append,  ///< Append the package to the destination file.
		};

		///
		PackageWriter(const StaticString& name, PackageType type = PackageType::Auto, Mode mode = Rewrite, Allocator* allocator = DefaultAllocator);

		///
		File open_file(const StaticString& name);

	private:
		Y_SHARED_PRIVATE(PackageWriter);
	};

	/// Package manager.
	class Y_API PackageManager
	{
	public:

		/// File search order.
		enum class Order
		{
			Preset,      ///< Use the PackageManager order.
			PackedFirst, ///< Try packed, then system file.
			SystemOnly,  ///< Try system file only.
			SystemFirst, ///< Try system, then packed file.
			PackedOnly,  ///< Try packed file only.
		};

	public:

		///
		PackageManager(Allocator* allocator = DefaultAllocator);

		///
		~PackageManager();

		/// Mount the specified package into the file system.
		/// \param name The package name.
		/// \param type Package file type.
		/// \return \c true on success.
		bool mount(const StaticString& name, PackageType type = PackageType::Auto);

		/// Open a file.
		/// \param name %File name.
		/// \param mode %File access mode.
		/// \param order The order to look for the file with.
		/// \return File.
		File open_file(const StaticString& name, unsigned mode = File::Read, Order order = Order::Preset);

		/// Set the default file system search order.
		/// \param order The order to be set.
		void set_order(Order order);

		/// Unmount all the mounted packages in the file system.
		void unmount_all();

	private:
		Y_MANAGER_PRIVATE(PackageManager);
	};
}

#endif // __Y_PACKAGE_H
