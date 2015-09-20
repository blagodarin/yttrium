/// \file
/// \brief Translation file.

#ifndef _include_yttrium_i18n_translation_h_
#define _include_yttrium_i18n_translation_h_

#include <yttrium/types.h>

namespace Yttrium
{
	class Localization;
	class StaticString;
	template <typename> class Pointer;

	///
	class Y_API Translation
	{
	public:

		///
		static Pointer<Translation> open(const StaticString& file_name, Allocator* allocator = DefaultAllocator);

		Translation() = default;
		virtual ~Translation() = default;

		///
		virtual void add(const StaticString& source) = 0;

		///
		virtual void remove_obsolete() = 0;

		///
		virtual bool save(const StaticString& file_name) const = 0;
	};
}

#endif
