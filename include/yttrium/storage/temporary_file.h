/// \file
/// \brief

#ifndef _include_yttrium_storage_temporary_file_h_
#define _include_yttrium_storage_temporary_file_h_

#include <yttrium/global.h>

#include <memory>

namespace Yttrium
{
	class StaticString;

	///
	class Y_API TemporaryFile
	{
	public:
		///
		TemporaryFile();

		///
		~TemporaryFile();

		///
		StaticString name() const;

	private:
		const std::unique_ptr<class TemporaryFilePrivate> _private;
	};
}

#endif
