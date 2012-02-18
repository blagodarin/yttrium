/// \file
/// \brief

#ifndef __Y_FILE_SYSTEM_HPP
#define __Y_FILE_SYSTEM_HPP

#include <vector>

#include <Yttrium/static_string.hpp>
#include <Yttrium/file.hpp>
#include <Yttrium/package.hpp>

namespace Yttrium
{

/// File system manager.

class FileSystem
{
public:

	/// File search order.

	// NOTE: 'System' name doesn't look like the best choice.

	enum Order
	{
		PresetOrder, ///< Use the file system defined order.
		PackedFirst, ///< Try packed, then system file.
		SystemOnly,  ///< Try system file only.
		SystemFirst, ///< Try system, then packed file.
		PackedOnly,  ///< Try packed file only.
	};

public:

	///

	FileSystem() noexcept
		: _order(PackedFirst)
	{
	}

	///

	~FileSystem() noexcept
	{
		unmount_all();
	}

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

	/// Unmount all the mounted packages from the file system.

	void unmount_all() noexcept;

public:

	/// Return the global FileSystem instance.

	static FileSystem &instance() noexcept;

private:

	File open_packed(const StaticString &name) const;

private:

	typedef std::vector<PackageReader *> Packages;

private:

	Packages _packages;
	Order    _order;
};

} // namespace Yttrium

#endif // __Y_FILE_SYSTEM_HPP
