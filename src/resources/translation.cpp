#include <yttrium/resources/translation.h>

#include <yttrium/exceptions.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/resources/resource_ptr.h>
#include <yttrium/std/map.h>
#include <yttrium/storage/reader.h>
#include <yttrium/string.h>

namespace Yttrium
{
	class TranslationImpl : public Translation
	{
	public:
		TranslationImpl(const Reader&, Allocator&);

		void add(const StaticString& source) override;
		void remove_obsolete() override;
		bool save(const StaticString& path) const override;
		String translate(const StaticString& source) const override;

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

	TranslationImpl::TranslationImpl(const Reader& reader, Allocator& allocator)
		: _allocator(allocator)
		, _translations(_allocator)
	{
		const auto document = IonDocument::open(reader, _allocator);
		if (!document)
			throw DataError("Bad translation data");
		decltype(_translations) translations(_allocator);
		for (const auto& node : document->root())
		{
			if (node.name() != "tr"_s || node.size() != 2)
				throw DataError("Bad translation data");
			const StaticString* source = nullptr;
			const StaticString* translation = nullptr;
			if (!node.first()->get(&source) || !node.last()->get(&translation))
				throw DataError("Bad translation data");
			translations.emplace(String(*source, &_allocator), String(*translation, &_allocator));
		}
		_translations = std::move(translations);
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
		const auto document = IonDocument::create(_allocator);
		for (const auto& translation : _translations)
		{
			auto& node = *document->root().append("tr"_s);
			node.append(translation.first);
			node.append(translation.second.text);
		}
		return document->save(file_name);
	}

	String TranslationImpl::translate(const StaticString& source) const
	{
		const auto i = _translations.find(String(source, ByReference(), &_allocator));
		return i != _translations.end() && !i->second.text.is_empty() ? i->second.text : String(source, &_allocator);
	}

	ResourcePtr<Translation> Translation::open(const Reader& reader, Allocator& allocator)
	{
		return reader ? make_resource<TranslationImpl>(reader, allocator) : nullptr;
	}
}