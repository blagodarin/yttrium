#ifndef _src_i18n_translation_h_
#define _src_i18n_translation_h_

#include <yttrium/i18n/translation.h>

#include <yttrium/string.h>

#include <map>

namespace Yttrium
{
	class TranslationImpl : public Translation
	{
	public:

		TranslationImpl(Allocator&);

		void add(const StaticString& source) override;
		void remove_obsolete() override;
		bool save(const StaticString& file_name) const override;

		bool load(const StaticString& file_name);
		String translate(const StaticString& source) const;

	private:

		struct Entry
		{
			String text;
			bool added = false;

			Entry(String&& text) : text(std::move(text)) {}
		};

		Allocator& _allocator;
		std::map<String, Entry> _translations;
	};
}

#endif
