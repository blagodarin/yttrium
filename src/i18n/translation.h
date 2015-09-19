#ifndef _src_i18n_translation_h_
#define _src_i18n_translation_h_

#include <yttrium/i18n/translation.h>

#include "../base/private_base.h"

#include <map>

namespace Yttrium
{
	class String;

	class Y_PRIVATE Translation::Private : public PrivateBase<Translation::Private>
	{
	public:

		Private(Allocator* allocator);

		void add(const StaticString& source);
		bool load(const StaticString& file_name);
		bool save(const StaticString& file_name) const;
		String translate(const StaticString& source) const;

	private:

		std::map<String, String> _translations;
	};
}

#endif
