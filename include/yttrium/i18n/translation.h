/// \file
/// \brief Translation file.

#ifndef _include_yttrium_i18n_translation_h_
#define _include_yttrium_i18n_translation_h_

#include <yttrium/memory/global.h>

#include <memory>

namespace Yttrium
{
	class Reader;
	class StaticString;
	class String;

	///
	class Y_API Translation
	{
	public:
		///
		static std::unique_ptr<Translation> open(const Reader&, Allocator& = *DefaultAllocator);

		Translation() = default;
		virtual ~Translation() = default;

		///
		virtual void add(const StaticString& source) = 0;

		///
		virtual void remove_obsolete() = 0;

		///
		virtual bool save(const StaticString& path) const = 0;

		///
		virtual String translate(const StaticString& source) const = 0;
	};
}

#endif
