/// \file
/// \brief Localization.

#ifndef __Y_I18N_LOCALIZATION_H
#define __Y_I18N_LOCALIZATION_H

#include <yttrium/global.h>

namespace Yttrium
{
	class Allocator;
	class StaticString;
	template <typename> class Pointer;

	///
	class Y_API Localization
	{
	public:

		///
		static Pointer<Localization> create(const StaticString& file_name, Allocator* allocator = nullptr);

		virtual ~Localization() = default;
	};
}

#endif
