/// \file
/// \brief Localization.

#ifndef __Y_I18N_LOCALIZATION_H
#define __Y_I18N_LOCALIZATION_H

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
