/// \file
/// \brief

#ifndef __Y_PACKAGE_H
#define __Y_PACKAGE_H

#include <yttrium/file.h>

#include <vector>

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

	PackageReader() noexcept
		: _private(nullptr)
	{
	}

	///

	PackageReader(const StaticString &name, PackageType type = PackageType::Auto, Allocator *allocator = DefaultAllocator) noexcept
		//: PackageReader() // TODO: Uncomment.
		: _private(nullptr)
	{
		open(name, type, allocator);
	}

	///

	PackageReader(const PackageReader &reader) noexcept;

	///

	~PackageReader() noexcept
	{
		close();
	}

public:

	///

	void close() noexcept;

	///

	bool is_opened() const noexcept
	{
		return _private;
	}

	///

	bool open(const StaticString &name, PackageType type = PackageType::Auto, Allocator *allocator = DefaultAllocator) noexcept;

	///

	File open_file(const StaticString &name) noexcept;

public:

	///

	PackageReader &operator =(const PackageReader &reader) noexcept;

public:

	class Private;

private:

	Private *_private;
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

public:

	///

	PackageWriter() noexcept
		: _private(nullptr)
	{
	}

	///

	PackageWriter(const StaticString &name, PackageType type = PackageType::Auto, Mode mode = Rewrite, Allocator *allocator = DefaultAllocator) noexcept
		//: PackageWriter() // TODO: Uncomment.
		: _private(nullptr)
	{
		open(name, type, mode, allocator);
	}

	///

	PackageWriter(const PackageWriter &writer) noexcept;

	///

	~PackageWriter() noexcept
	{
		close();
	}

public:

	///

	void close() noexcept;

	///

	bool is_opened() const noexcept
	{
		return _private;
	}

	///

	bool open(const StaticString &name, PackageType type = PackageType::Auto, Mode mode = Rewrite, Allocator *allocator = DefaultAllocator) noexcept;

	///

	File open_file(const StaticString &name) noexcept;

public:

	///

	PackageWriter &operator =(const PackageWriter &writer) noexcept;

public:

	class Private;

private:

	Private *_private;
};

/// Package manager.

class Y_API PackageManager
{
	Y_NONCOPYABLE(PackageManager);

public:

	/// File search order.

	enum Order // TODO: Invent better names for 'System*' constants.
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
	/// \param type Package file type.
	/// \return \c true on success.

	bool mount(const StaticString &name, PackageType type = PackageType::Auto) noexcept;

	/// Open a file.
	/// \param name %File name.
	/// \param mode %File access mode.
	/// \param order The order to look for the file with.
	/// \return File.

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

	Y_PRIVATE File open_packed(const StaticString &name) const;

private:

	typedef std::vector<PackageReader> Packages;

private:

	Allocator        *_allocator;
	mutable Packages  _packages;
	Order             _order;
};

} // namespace Yttrium

#endif // __Y_PACKAGE_H
