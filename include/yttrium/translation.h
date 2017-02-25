/// \file
/// \brief

#ifndef _include_yttrium_translation_h_
#define _include_yttrium_translation_h_

#include <yttrium/api.h>

#include <memory>
#include <string>

namespace Yttrium
{
	class Reader;
	class StaticString;

	/// Translation file.
	class Y_API Translation
	{
	public:
		///
		/// Returns 'nullptr' for a null Reader.
		/// May throw DataError.
		static std::unique_ptr<Translation> open(const Reader&);

		///
		virtual void add(const StaticString& source) = 0;

		///
		virtual void remove_obsolete() = 0;

		///
		virtual bool save(const std::string& path) const = 0;

		///
		virtual std::string translate(const StaticString& source) const = 0;
	};
}

#endif
