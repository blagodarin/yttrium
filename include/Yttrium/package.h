/// \file
/// \brief

#ifndef __Y_PACKAGE_H
#define __Y_PACKAGE_H

#include <vector> // std::vector

#include <Yttrium/allocator.h>
#include <Yttrium/file.h>
#include <Yttrium/noncopyable.h>
#include <Yttrium/static_string.h>

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

	File open_file(const StaticString &name, Allocator *allocator = DefaultAllocator) noexcept;

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

	///

	PackageWriter() noexcept
		: _private(nullptr)
	{
	}

	///

	PackageWriter(const StaticString &name, PackageType type = PackageType::Auto, Allocator *allocator = DefaultAllocator) noexcept
		//: PackageWriter() // TODO: Uncomment.
		: _private(nullptr)
	{
		open(name, type, allocator);
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

	bool open(const StaticString &name, PackageType type = PackageType::Auto, Allocator *allocator = DefaultAllocator) noexcept;

	///

	File open_file(const StaticString &name, Allocator *allocator = DefaultAllocator) noexcept;

public:

	///

	PackageWriter &operator =(const PackageWriter &writer) noexcept;

public:

	class Private;

private:

	Private *_private;
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

	bool mount(const StaticString &name, PackageType type = PackageType::Auto) noexcept;

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

	typedef std::vector<PackageReader> Packages;

private:

	Allocator        *_allocator;
	mutable Packages  _packages;
	Order             _order;
};

} // namespace Yttrium

#endif // __Y_PACKAGE_H
