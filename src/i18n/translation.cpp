#include "translation.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/pointer.h>

namespace Yttrium
{
	TranslationImpl::TranslationImpl(Allocator* allocator)
		: _allocator(allocator)
	{
	}

	void TranslationImpl::add(const StaticString& source)
	{
		auto i = _translations.find(String(source, ByReference(), _allocator));
		if (i == _translations.end())
			i = _translations.emplace(String(source, _allocator), String(_allocator)).first;
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
			auto& node = *document.root().append(S("tr"));
			node.append(translation.first);
			node.append(translation.second.text);
		}
		return document.save(file_name);
	}

	bool TranslationImpl::load(const StaticString& file_name)
	{
		IonDocument document(_allocator);
		if (!document.load(file_name))
			return false;
		decltype(_translations) translations;
		const auto& root = document.root();
		for (const auto& node : root)
		{
			if (node.name() != S("tr") || node.size() != 2)
				return false;
			const StaticString* source = nullptr;
			const StaticString* translation = nullptr;
			if (!node.first()->get(&source) || !node.last()->get(&translation))
				return false;
			translations.emplace(String(*source, _allocator), String(*translation, _allocator));
		}
		_translations = std::move(translations);
		return true;
	}

	String TranslationImpl::translate(const StaticString& source) const
	{
		const auto i = _translations.find(String(source, ByReference(), _allocator));
		return i != _translations.end() && !i->second.text.is_empty() ? i->second.text : String(source, _allocator);
	}

	Pointer<Translation> Translation::open(const StaticString& file_name, Allocator* allocator)
	{
		auto translation = make_pointer<TranslationImpl>(*allocator, allocator);
		translation->load(file_name);
		return std::move(translation);
	}
}
