/// \file
/// \brief Localization.

#ifndef _include_yttrium_i18n_localization_h_
#define _include_yttrium_i18n_localization_h_

#include <yttrium/memory/global.h>

namespace Yttrium
{
	class Allocator;
	class StaticString;
	class String;

	template <typename> class UniquePtr;

	///
	class Y_API Localization
	{
	public:

		///
		static UniquePtr<Localization> create(const StaticString& file_name, Allocator& allocator = *DefaultAllocator);

		///
		static String localize(const StaticString& source);

		Localization() = default;
		virtual ~Localization() = default;
	};
}

#endif
