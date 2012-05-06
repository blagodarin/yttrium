/// \file
/// \brief

#ifndef __Y_PACKAGE_HPP
#define __Y_PACKAGE_HPP

#include <vector> // std::vector

#include <Yttrium/allocator.hpp>
#include <Yttrium/file.hpp>
#include <Yttrium/noncopyable.hpp>
#include <Yttrium/static_string.hpp>

namespace Yttrium
{

/// Supported package formats.

enum class PackageFormat
{
	Auto,    ///< Automatically detect package format.
	Yttrium, ///< Yttrium packages.
};

/// Package reader class.

class Y_API PackageReader
{
public:

	virtual ~PackageReader() noexcept
	{
	}

	///

	virtual File open_file(const StaticString &name, Allocator *allocator = DefaultAllocator) noexcept = 0;

public:

	///

	static PackageReader *open(const StaticString &name, PackageFormat format = PackageFormat::Auto) noexcept;
};

/// Package writer class.

class Y_API PackageWriter
{
public:

	virtual ~PackageWriter() noexcept
	{
	}

	///

	virtual File open_file(const StaticString &name, Allocator *allocator = DefaultAllocator) noexcept = 0;

public:

	///

	static PackageWriter *open(const StaticString &name, PackageFormat format) noexcept;
};

/// Package manager.

class Y_API PackageManager: public Noncopyable
{
public:

	/// File search order.

	enum Order // NOTE: 'System*' names don't look the best.
	{
		PresetOrder, ///< Use the PackageManager order.
		PackedFirst, ///< Try packed, then system file.
		SystemOnly,  ///< Try system file only.
		SystemFirst, ///< Try system, then packed file.
		PackedOnly,  ///< Try packed file only.
	};

public:

	///

	PackageManager(Allocator *allocator = DefaultAllocator) noexcept;

	///

	~PackageManager() noexcept;

public:

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

	/// Return the global PackageManager instance.

	static PackageManager *instance() noexcept;

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

#endif // __Y_PACKAGE_HPP
