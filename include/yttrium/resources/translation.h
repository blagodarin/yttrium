/// \file
/// \brief

#ifndef _include_yttrium_resources_translation_h_
#define _include_yttrium_resources_translation_h_

#include <yttrium/memory/global.h>
#include <yttrium/resources/resource.h>

namespace Yttrium
{
	class Reader;
	template <typename> class ResourcePtr;
	class StaticString;
	class String;

	/// Translation file.
	class Y_API Translation : public Resource
	{
	public:
		///
		static ResourcePtr<Translation> open(const Reader&, Allocator& = *DefaultAllocator);

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
