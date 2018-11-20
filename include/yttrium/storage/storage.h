//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef _include_yttrium_storage_storage_h_
#define _include_yttrium_storage_storage_h_

#include <yttrium/storage/package.h>

#include <memory>

namespace Yttrium
{
	class Buffer;
	class Source;

	///
	class Y_CORE_API Storage
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
