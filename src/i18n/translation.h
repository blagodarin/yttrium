#ifndef _src_i18n_translation_h_
#define _src_i18n_translation_h_

#include <yttrium/i18n/translation.h>

#include <yttrium/string.h>
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
		void remove_obsolete();
		bool save(const StaticString& file_name) const;
		String translate(const StaticString& source) const;

	private:

		struct Entry
		{
			String text;
			bool added = false;

			Entry(String&& text) : text(std::move(text)) {}
		};

		std::map<String, Entry> _translations;
	};
}

#endif
