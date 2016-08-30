#include "classes.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/utils.h>

namespace Yttrium
{
	namespace
	{
		void update_document(IonObject& target, const IonObject& source)
		{
			for (auto i = source.rbegin(); i != source.rend(); ++i)
			{
				if (!target.contains(i->name()))
					Ion::append(target, *i);
			}
		}
	}

	GuiClasses::GuiClasses(Allocator& allocator)
		: _allocator(allocator)
		, _classes(_allocator)
	{
	}

	GuiClasses::~GuiClasses() = default;

	bool GuiClasses::add(const StaticString& name, const IonObject& source, const StaticString* base_class)
	{
		if (name.is_empty() || _classes.find(String(name, ByReference())) != _classes.end())
			return false;

		IonObject* base = nullptr;
		if (base_class)
		{
			auto i = _classes.find(String(*base_class, ByReference()));
			if (i == _classes.end())
				return false;
			base = &i->second.root();
		}

		IonDocument document(_allocator);
		update_document(document.root(), source);
		if (base)
			update_document(document.root(), *base);

		_classes.emplace(String(name, &_allocator), std::move(document));

		return true;
	}

	void GuiClasses::clear()
	{
		_classes.clear();
	}

	const IonObject* GuiClasses::find(const StaticString& name) const
	{
		auto i = _classes.find(String(name, ByReference()));
		return i != _classes.end() ? &i->second.root() : nullptr;
	}
}
