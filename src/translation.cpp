#include <yttrium/translation.h>

#include <yttrium/exceptions.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
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
		bool save(const std::string&) const override;
		std::string translate(const StaticString& source) const override;

	private:
		struct Entry
		{
			std::string text;
			bool added = false;

			Entry() = default;
			Entry(std::string&& text) : text(std::move(text)) {}
		};

		Allocator& _allocator;
		std::map<String, Entry> _translations;
	};

	TranslationImpl::TranslationImpl(const Reader& reader, Allocator& allocator)
		: _allocator(allocator)
	{
		const auto document = IonDocument::open(reader, _allocator);
		if (!document)
			throw DataError("Bad translation data");
		decltype(_translations) translations;
		for (const auto& node : document->root())
		{
			if (node.name() != "tr"_s || node.size() != 2)
				throw DataError("Bad translation data");
			const StaticString* source = nullptr;
			const StaticString* translation = nullptr;
			if (!node.first()->get(&source) || !node.last()->get(&translation))
				throw DataError("Bad translation data");
			translations.emplace(String(*source, &_allocator), translation->to_std());
		}
		_translations = std::move(translations);
	}

	void TranslationImpl::add(const StaticString& source)
	{
		auto i = _translations.find(String(source, ByReference(), &_allocator));
		if (i == _translations.end())
			i = _translations.emplace(String(source, &_allocator), Entry{}).first;
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

	bool TranslationImpl::save(const std::string& path) const
	{
		const auto document = IonDocument::create(_allocator);
		for (const auto& translation : _translations)
		{
			auto& node = *document->root().append("tr"_s);
			node.append(translation.first);
			node.append(StaticString{ translation.second.text });
		}
		return document->save(path);
	}

	std::string TranslationImpl::translate(const StaticString& source) const
	{
		const auto i = _translations.find(String(source, ByReference(), &_allocator));
		return i != _translations.end() && !i->second.text.empty() ? i->second.text : source.to_std();
	}

	std::unique_ptr<Translation> Translation::open(const Reader& reader, Allocator& allocator)
	{
		return reader ? std::make_unique<TranslationImpl>(reader, allocator) : nullptr;
	}
}
