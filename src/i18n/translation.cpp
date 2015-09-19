#include <yttrium/i18n/translation.h>

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/string.h>
#include "../base/private_base.h"

#include <map>

namespace Yttrium
{
	class Translation::Private : public PrivateBase<Translation::Private>
	{
	public:

		Private(Allocator* allocator)
			: PrivateBase(allocator)
		{
		}

		void add(const StaticString& source)
		{
			_translations[String(source, _allocator)];
		}

		bool load(const StaticString& file_name)
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

		bool save(const StaticString& file_name) const
		{
			IonDocument document(_allocator);
			for (const auto& translation : _translations)
			{
				auto& node = *document.root().append(S("tr"));
				node.append(translation.first);
				node.append(translation.second);
			}
			return document.save(file_name);
		}

	private:

		std::map<String, String> _translations;
	};

	Y_IMPLEMENT_UNIQUE(Translation);

	Translation::Translation(const StaticString& file_name, Allocator* allocator)
	{
		PrivateHolder<Translation::Private> translation(allocator);
		translation->load(file_name);
		_private = translation.release();
	}

	void Translation::add(const StaticString& source)
	{
		_private->add(source);
	}

	bool Translation::save(const StaticString& file_name) const
	{
		return _private->save(file_name);
	}
}
