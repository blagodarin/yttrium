/// \file
/// \brief

#ifndef __Y_FILE_SYSTEM_HPP
#define __Y_FILE_SYSTEM_HPP

#include <vector> // std::vector

#include <Yttrium/allocator.hpp>
#include <Yttrium/static_string.hpp>
#include <Yttrium/file.hpp>
#include <Yttrium/noncopyable.hpp>
#include <Yttrium/package.hpp>

namespace Yttrium
{

/// File system manager.

class Y_API FileSystem: public Noncopyable
{
public:

	/// File search order.

	enum Order // NOTE: 'System*' names don't look the best.
	{
		PresetOrder, ///< Use the file system defined order.
		PackedFirst, ///< Try packed, then system file.
		SystemOnly,  ///< Try system file only.
		SystemFirst, ///< Try system, then packed file.
		PackedOnly,  ///< Try packed file only.
	};

public:

	///

	FileSystem(Allocator *allocator = HeapAllocator::instance()) noexcept;

	///

	~FileSystem() noexcept;

	/// Mount the specified package into the file system.
	/// \param name The package name.
	/// \param format

	bool mount(const StaticString &name, PackageFormat format = PackageFormat::Auto) noexcept;

	/// Open a file.
	/// \param name %File name.
	/// \param order The order to look for the file with.

	File open_file(const StaticString &name, File::Mode mode = File::Read, Order order = PresetOrder) noexcept;

	/// Set the default file system search order.
	/// \param order The order to be set.

	void set_order(Order order) noexcept
	{
		if (order != PresetOrder)
		{
			_order = order;
		}
	}

	/// Unmount all the mounted packages in the file system.

	void unmount_all() noexcept;

public:

	/// Return the global FileSystem instance.

	static FileSystem *instance() noexcept;

private:

	File open_packed(const StaticString &name) const;

private:

	typedef std::vector<PackageReader *> Packages;

private:

	Allocator *_allocator;
	Packages   _packages;
	Order      _order;
};

} // namespace Yttrium

#endif // __Y_FILE_SYSTEM_HPP
