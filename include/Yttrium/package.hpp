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

	virtual ~PackageReader()
	{
	}

	///

	virtual FileReaderPtr open_reader(const StaticString &name) = 0;

public:

	///

	static PackageReader *open(const StaticString &name, PackageFormat format = PackageFormat::Auto);
};

/// Package writer class.

class PackageWriter
{
public:

	virtual ~PackageWriter()
	{
	}

	///

	virtual FileWriterPtr open_writer(const StaticString &name) = 0;

public:

	///

	static PackageWriter *open(const StaticString &name, PackageFormat format);
};

} // namespace Yttrium

#endif // __Y_PACKAGE_HPP
