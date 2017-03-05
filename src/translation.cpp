#include <yttrium/translation.h>

#include <yttrium/exceptions.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/ion/writer.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>
#include <yttrium/string.h>

#include <algorithm>

namespace Yttrium
{
	class TranslationImpl : public Translation
	{
	public:
		TranslationImpl(const Reader&);

		void add(const StaticString& source) override;
		void remove_obsolete() override;
		void save(const std::string&) const override;
		std::string translate(const StaticString& source) const override;

	private:
		struct Entry
		{
			std::string text;
			bool added = false;

			Entry() = default;
			Entry(std::string&& text_) : text(std::move(text_)) {}
		};

		std::map<String, Entry> _translations;
	};

	TranslationImpl::TranslationImpl(const Reader& reader)
	{
		const auto document = IonDocument::open(reader);
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
			translations.emplace(String{ *source }, translation->to_std());
		}
		_translations = std::move(translations);
	}

	void TranslationImpl::add(const StaticString& source)
	{
		auto i = _translations.find({ source, ByReference{} });
		if (i == _translations.end())
			i = _translations.emplace(String{ source }, Entry{}).first;
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

	void TranslationImpl::save(const std::string& path) const
	{
		std::vector<std::pair<StaticString, StaticString>> translations;
		translations.reserve(_translations.size());
		std::for_each(_translations.begin(), _translations.end(), [&translations](const auto& t){ translations.emplace_back(t.first, StaticString{ t.second.text }); });
		std::sort(translations.begin(), translations.end(), [](const auto& a, const auto& b){ return a.first < b.first; });
		Writer writer{ path };
		IonWriter ion{ writer, IonWriter::Formatting::Pretty };
		for (const auto& translation : translations)
		{
			ion.add_name("tr"_s);
			ion.add_value(translation.first);
			ion.add_value(translation.second);
		}
		ion.flush();
	}

	std::string TranslationImpl::translate(const StaticString& source) const
	{
		const auto i = _translations.find({ source, ByReference{} });
		return i != _translations.end() && !i->second.text.empty() ? i->second.text : source.to_std();
	}

	std::unique_ptr<Translation> Translation::open(const Reader& reader)
	{
		return reader ? std::make_unique<TranslationImpl>(reader) : nullptr;
	}
}
