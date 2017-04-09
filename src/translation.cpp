#include <yttrium/translation.h>

#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
#include <yttrium/ion/writer.h>
#include <yttrium/storage/writer.h>
#include <yttrium/string.h>

#include <algorithm>
#include <map>
#include <vector>

namespace Yttrium
{
	class TranslationImpl : public Translation
	{
	public:
		explicit TranslationImpl(const Source&);

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
			explicit Entry(std::string&& text_) : text(std::move(text_)) {}
		};

		std::map<String, Entry> _translations;
	};

	TranslationImpl::TranslationImpl(const Source& source)
	{
		IonReader ion{source};
		decltype(_translations) translations;
		for (auto token = ion.read(); token.type() != IonReader::Token::Type::End; token = ion.read())
		{
			token.check_name("tr"_s);
			const auto text = ion.read().to_value();
			const auto translation = ion.read().to_value();
			translations.emplace(String{text}, Entry{translation.to_std()});
		}
		_translations = std::move(translations);
	}

	void TranslationImpl::add(const StaticString& text)
	{
		auto i = _translations.find({text, ByReference{}});
		if (i == _translations.end())
			i = _translations.emplace(String{text}, Entry{}).first;
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
		std::for_each(_translations.begin(), _translations.end(), [&translations](const auto& t){ translations.emplace_back(t.first, StaticString{t.second.text}); });
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
		const auto i = _translations.find({source, ByReference{}});
		return i != _translations.end() && !i->second.text.empty() ? i->second.text : source.to_std();
	}

	std::unique_ptr<Translation> Translation::load(const Source& source)
	{
		return std::make_unique<TranslationImpl>(source);
	}
}
