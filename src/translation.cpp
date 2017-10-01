#include <yttrium/translation.h>

#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
#include <yttrium/ion/writer.h>
#include <yttrium/storage/writer.h>

#include <algorithm>
#include <map>
#include <vector>

namespace Yttrium
{
	class TranslationImpl : public Translation
	{
	public:
		explicit TranslationImpl(const Source&);

		void add(std::string_view) override;
		void remove_obsolete() override;
		bool save(const std::string&) const override;
		std::string translate(std::string_view) const override;

	private:
		struct Entry
		{
			std::string _text;
			bool _added = false;

			Entry() = default;
			explicit Entry(std::string_view text) : _text{text} {}
		};

		std::map<std::string, Entry, std::less<>> _translations;
	};

	TranslationImpl::TranslationImpl(const Source& source)
	{
		IonReader ion{source};
		decltype(_translations) translations;
		for (auto token = ion.read(); token.type() != IonReader::Token::Type::End; token = ion.read())
		{
			token.check_name("tr");
			const auto text = ion.read().to_value();
			const auto translation = ion.read().to_value();
			translations.emplace(text, Entry{translation});
		}
		_translations = std::move(translations);
	}

	void TranslationImpl::add(std::string_view text)
	{
		_translations[std::string{text}]._added = true;
	}

	void TranslationImpl::remove_obsolete()
	{
		for (auto i = _translations.begin(); i != _translations.end();)
			if (i->second._added)
				++i;
			else
				i = _translations.erase(i);
	}

	bool TranslationImpl::save(const std::string& path) const
	{
		std::vector<std::pair<std::string_view, std::string_view>> translations;
		translations.reserve(_translations.size());
		std::for_each(_translations.begin(), _translations.end(), [&translations](const auto& t){ translations.emplace_back(t.first, t.second._text); });
		std::sort(translations.begin(), translations.end(), [](const auto& a, const auto& b){ return a.first < b.first; });
		Writer writer{path};
		if (!writer)
			return false;
		IonWriter ion{writer, IonWriter::Formatting::Pretty};
		for (const auto& translation : translations)
		{
			ion.add_name("tr");
			ion.add_value(translation.first);
			ion.add_value(translation.second);
		}
		ion.flush();
		return true;
	}

	std::string TranslationImpl::translate(std::string_view source) const
	{
		const auto i = _translations.find(source);
		return i != _translations.end() && !i->second._text.empty() ? i->second._text : std::string{source};
	}

	std::unique_ptr<Translation> Translation::load(const Source& source)
	{
		return std::make_unique<TranslationImpl>(source);
	}
}
