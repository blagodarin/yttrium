/// \file
/// \brief

#ifndef _include_yttrium_storage_package_h_
#define _include_yttrium_storage_package_h_

#include <yttrium/api.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Yttrium
{
	class Source;
	class StaticString;

	/// Package file types.
	enum class PackageType
	{
		Auto, ///< Automatical detection.
		Ypq,  ///< YPQ package.
	};

	/// Package reader interface.
	class Y_API PackageReader
	{
	public:
		///
		static std::unique_ptr<PackageReader> create(const std::string& path, PackageType = PackageType::Auto);

		///
		virtual ~PackageReader() = default;

		///
		virtual const std::vector<StaticString>& names() const = 0;

		///
		virtual std::unique_ptr<Source> open(std::size_t index) const = 0;
	};

	/// Package writer interface.
	class Y_API PackageWriter
	{
	public:
		///
		static std::unique_ptr<PackageWriter> create(const std::string& path, PackageType = PackageType::Auto);

		///
		virtual ~PackageWriter() = default;

		///
		virtual bool add(const std::string&, std::map<std::string, std::string>&& properties) = 0;

		///
		virtual bool commit() = 0;
	};
}

#endif
