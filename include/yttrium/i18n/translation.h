/// \file
/// \brief Translation file.

#ifndef __Y_I18N_TRANSLATION_H
#define __Y_I18N_TRANSLATION_H

#include <yttrium/base.h>
#include <yttrium/types.h>

namespace Yttrium
{
	class StaticString;

	///
	class Y_API Translation
	{
	public:

		///
		Translation(const StaticString& file_name, Allocator* allocator = DefaultAllocator);

		///
		void add(const StaticString& source);

		///
		bool save(const StaticString& file_name) const;

	private:
		Y_UNIQUE_PRIVATE(Translation);
	};
}

#endif
