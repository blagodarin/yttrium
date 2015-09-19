/// \file
/// \brief Translation file.

#ifndef _include_yttrium_i18n_translation_h_
#define _include_yttrium_i18n_translation_h_

#include <yttrium/base.h>
#include <yttrium/types.h>

namespace Yttrium
{
	class Localization;
	class StaticString;

	///
	class Y_API Translation
	{
		friend Localization;
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
