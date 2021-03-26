// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/i18n/translation.h>

#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
#include <yttrium/ion/writer.h>
#include <yttrium/storage/writer.h>

#include <algorithm>
#include <map>
#include <vector>

namespace Yt
{
	class TranslationImpl : public Translation
	{
	public:
		explicit TranslationImpl(const Source&);

		void add(std::string_view) override;
		bool changed() const noexcept override { return _changed; }
		void remove_obsolete() override;
		void save(Writer&&) const override;
		std::string translate(std::string_view) const override;

	private:
		struct Entry
		{
			std::string _text;
			bool _added = false;

			Entry() = default;
			explicit Entry(std::string_view text)
				: _text{ text } {}
		};

		std::map<std::string, Entry, std::less<>> _translations;
		bool _changed = false;
	};

	TranslationImpl::TranslationImpl(const Source& source)
	{
		IonReader ion{ source };
		decltype(_translations) translations;
		for (auto token = ion.read(); token.type() != IonToken::Type::End; token = ion.read())
		{
			token.check_name("tr");
			const auto text = ion.read().to_value();
			const auto translation = ion.read().to_value(); //-V656
			translations.emplace(text, Entry{ translation });
		}
		_translations = std::move(translations);
	}

	void TranslationImpl::add(std::string_view text)
	{
		const auto [i, inserted] = _translations.try_emplace(std::string{ text });
		i->second._added = true;
		if (inserted)
			_changed = true;
	}

	void TranslationImpl::remove_obsolete()
	{
		for (auto i = _translations.begin(); i != _translations.end();)
			if (!i->second._added)
			{
				i = _translations.erase(i);
				_changed = true;
			}
			else
				++i;
	}

	void TranslationImpl::save(Writer&& writer) const
	{
		std::vector<std::pair<std::string_view, std::string_view>> translations;
		translations.reserve(_translations.size());
		std::for_each(_translations.begin(), _translations.end(), [&translations](const auto& t) { translations.emplace_back(t.first, t.second._text); });
		std::sort(translations.begin(), translations.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
		IonWriter ion{ writer, IonWriter::Formatting::Pretty };
		for (const auto& [source, translation] : translations)
		{
			ion.add_name("tr");
			ion.add_value(source);
			ion.add_value(translation);
		}
		ion.flush();
	}

	std::string TranslationImpl::translate(std::string_view source) const
	{
		const auto i = _translations.find(source);
		return i != _translations.end() && !i->second._text.empty() ? i->second._text : std::string{ source };
	}

	std::unique_ptr<Translation> Translation::load(const Source& source)
	{
		return std::make_unique<TranslationImpl>(source);
	}
}
