/// \file
/// \brief

#ifndef __Y_PACKAGE_HPP
#define __Y_PACKAGE_HPP

#include <Yttrium/file.hpp>
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

class PackageReader
{
public:

	virtual ~PackageReader() noexcept
	{
	}

	///

	virtual File open_file(const StaticString &name) noexcept = 0;

public:

	///

	static PackageReader *open(const StaticString &name, PackageFormat format = PackageFormat::Auto) noexcept;
};

/// Package writer class.

class PackageWriter
{
public:

	virtual ~PackageWriter() noexcept
	{
	}

	///

	virtual File open_file(const StaticString &name) noexcept = 0;

public:

	///

	static PackageWriter *open(const StaticString &name, PackageFormat format) noexcept;
};

} // namespace Yttrium

#endif // __Y_PACKAGE_HPP
