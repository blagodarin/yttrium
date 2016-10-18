#include <yttrium/i18n/translation.h>

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/std/map.h>
#include <yttrium/string.h>

namespace Yttrium
{
	class TranslationImpl : public Translation
	{
	public:
		TranslationImpl(Allocator&);

		void add(const StaticString& source) override;
		void remove_obsolete() override;
		bool save(const StaticString& path) const override;
		String translate(const StaticString& source) const override;

		bool load(const Reader&);

	private:
		struct Entry
		{
			String text;
			bool added = false;

			Entry(String&& text) : text(std::move(text)) {}
		};

		Allocator& _allocator;
		StdMap<String, Entry> _translations;
	};

	TranslationImpl::TranslationImpl(Allocator& allocator)
		: _allocator(allocator)
		, _translations(_allocator)
	{
	}

	void TranslationImpl::add(const StaticString& source)
	{
		auto i = _translations.find(String(source, ByReference(), &_allocator));
		if (i == _translations.end())
			i = _translations.emplace(String(source, &_allocator), String(&_allocator)).first;
		i->second.added = true;
	}

	void TranslationImpl::remove_obsolete()
	{
		for (auto i = _translations.begin(); i != _translations.end(); )
			if (i->second.added)
				++i;
			else
				i = _translations.erase(i);
	}

	bool TranslationImpl::save(const StaticString& file_name) const
	{
		IonDocument document(_allocator);
		for (const auto& translation : _translations)
		{
			auto& node = *document.root().append("tr"_s);
			node.append(translation.first);
			node.append(translation.second.text);
		}
		return document.save(file_name);
	}

	bool TranslationImpl::load(const Reader& reader)
	{
		IonDocument document(_allocator);
		if (!document.load(reader))
			return false;
		decltype(_translations) translations(_allocator);
		const auto& root = document.root();
		for (const auto& node : root)
		{
			if (node.name() != "tr"_s || node.size() != 2)
				return false;
			const StaticString* source = nullptr;
			const StaticString* translation = nullptr;
			if (!node.first()->get(&source) || !node.last()->get(&translation))
				return false;
			translations.emplace(String(*source, &_allocator), String(*translation, &_allocator));
		}
		_translations = std::move(translations);
		return true;
	}

	String TranslationImpl::translate(const StaticString& source) const
	{
		const auto i = _translations.find(String(source, ByReference(), &_allocator));
		return i != _translations.end() && !i->second.text.is_empty() ? i->second.text : String(source, &_allocator);
	}

	std::unique_ptr<Translation> Translation::open(const Reader& reader, Allocator& allocator)
	{
		auto translation = std::make_unique<TranslationImpl>(allocator);
		return translation->load(reader) ? std::move(translation) : nullptr;
	}
}
