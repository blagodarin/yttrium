/// \file
/// \brief Localization.

#ifndef _include_yttrium_i18n_localization_h_
#define _include_yttrium_i18n_localization_h_

#include <yttrium/global.h>

namespace Yttrium
{
	class Allocator;
	class StaticString;
	class String;
	template <typename> class Pointer;

	///
	class Y_API Localization
	{
	public:

		///
		static Pointer<Localization> create(const StaticString& file_name, Allocator* allocator = nullptr);

		///
		static String localize(const String& source);

		virtual ~Localization() = default;
	};
}

#endif
