/// \file
/// \brief

#ifndef _include_yttrium_storage_temporary_file_h_
#define _include_yttrium_storage_temporary_file_h_

#include <yttrium/api.h>

#include <memory>
#include <string>

namespace Yttrium
{
	///
	class Y_API TemporaryFile
	{
	public:
		///
		TemporaryFile();

		///
		~TemporaryFile();

		///
		const std::string& name() const;

	private:
		const std::unique_ptr<class TemporaryFilePrivate> _private;
	};
}

#endif
