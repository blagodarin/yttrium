#ifndef _include_yttrium_storage_storage_h_
#define _include_yttrium_storage_storage_h_

#include <yttrium/storage/package.h>

#include <memory>

namespace Yttrium
{
	class Buffer;
	class Source;

	///
	class Y_API Storage
	{
	public:
		///
		enum class UseFileSystem
		{
			After,
			Before,
			Never,
		};

		///
		explicit Storage(UseFileSystem);

		///
		~Storage();

		/// Attaches a buffer to the storage.
		void attach_buffer(const std::string& name, Buffer&&);

		/// Attaches a package to the storage.
		/// Throws MissingDataError if there is no such package.
		void attach_package(const std::string& path, PackageType = PackageType::Auto);

		/// Opens a data source.
		std::unique_ptr<Source> open(std::string_view name) const;

	private:
		const std::unique_ptr<class StoragePrivate> _private;
	};
}

#endif
