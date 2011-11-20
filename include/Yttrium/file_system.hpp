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

	FileSystem() throw()
		: _order(PackedFirst)
	{
	}

	///

	~FileSystem() throw()
	{
		unmount_all();
	}

	/// Mount the specified package into the file system.
	/// \param name The package name.
	/// \param format

	bool mount(const StaticString &name, PackageFormat format = PackageFormat::Auto);

	/// Open a FileReader.
	/// \param name File name.
	/// \param order The order to look for the file with.

	FileReaderPtr open_reader(const StaticString &name, Order order = PresetOrder);

	/// Set the default file system search order.
	/// \param order The order to be set.

	void set_order(Order order) throw()
	{
		if (order != PresetOrder)
		{
			_order = order;
		}
	}

	/// Unmount all the mounted packages from the file system.

	void unmount_all() throw();

public:

	/// Return the global FileSystem instance.

	static FileSystem &instance() throw();

	/// Open a FileWriter.
	/// \param name File name.

	static FileWriterPtr open_writer(const StaticString &name)
	{
		return FileWriter::open(name);
	}

private:

	Order effective_order(Order order) const throw()
	{
		return (order == PresetOrder ? _order : order);
	}

	FileReaderPtr open_packed(const StaticString &name);

private:

	typedef std::vector<PackageReader *> Packages;

private:

	Packages _packages;
	Order    _order;
};

} // namespace Yttrium

#endif // __Y_FILE_SYSTEM_HPP
