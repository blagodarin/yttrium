/// \file
/// \brief

#ifndef _include_yttrium_storage_package_h_
#define _include_yttrium_storage_package_h_

#include <yttrium/memory/global.h>

#include <map>
#include <string>

namespace Yttrium
{
	class Reader;
	class StaticString;

	template <typename> class UniquePtr;

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
		static UniquePtr<PackageReader> create(const StaticString& path, PackageType = PackageType::Auto, Allocator& = *DefaultAllocator);

		PackageReader() = default;
		virtual ~PackageReader() = default;

		///
		virtual Reader open(const StaticString& name) const = 0;
	};

	/// Package writer class.
	class Y_API PackageWriter
	{
	public:
		///
		static UniquePtr<PackageWriter> create(const StaticString& path, PackageType = PackageType::Auto, Allocator& = *DefaultAllocator);

		PackageWriter() = default;
		virtual ~PackageWriter() = default;

		///
		virtual bool add(const StaticString&, std::map<std::string, std::string>&& properties) = 0;

		///
		virtual bool commit() = 0;
	};
}

#endif