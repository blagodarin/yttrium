#ifndef _include_yttrium_translation_h_
#define _include_yttrium_translation_h_

#include <yttrium/api.h>
#include <yttrium/std/string_view.h>

#include <memory>
#include <string>

namespace Yttrium
{
	class Source;

	/// Translation file.
	class Y_API Translation
	{
	public:
		///
		/// May throw DataError.
		static std::unique_ptr<Translation> load(const Source&);

		virtual ~Translation() = default;

		///
		virtual void add(std::string_view source) = 0;

		///
		virtual void remove_obsolete() = 0;

		///
		virtual void save(const std::string& path) const = 0;

		///
		virtual std::string translate(std::string_view source) const = 0;
	};
}

#endif
