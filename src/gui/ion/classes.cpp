#include "classes.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/utils.h>

namespace
{
	using namespace Yttrium;

	void update_document(IonObject& target, const IonObject& source)
	{
		for (auto i = source.rbegin(); i != source.rend(); ++i)
		{
			if (!target.contains(i->name()))
				Ion::append(target, *i);
		}
	}
}

namespace Yttrium
{
	GuiClasses::GuiClasses(Allocator& allocator)
		: _allocator(allocator)
	{
	}

	GuiClasses::~GuiClasses() = default;

	bool GuiClasses::add(const std::string& name, const IonObject& source, const std::string* base_class)
	{
		if (name.empty() || _classes.find(name) != _classes.end())
			return false;

		const IonObject* base = nullptr;
		if (base_class)
		{
			const auto i = _classes.find(*base_class);
			if (i == _classes.end())
				return false;
			base = &i->second->root();
		}

		auto document = IonDocument::create(_allocator);
		::update_document(document->root(), source);
		if (base)
			::update_document(document->root(), *base);

		_classes.emplace(name, std::move(document));

		return true;
	}

	void GuiClasses::clear()
	{
		_classes.clear();
	}

	const IonObject* GuiClasses::find(const std::string& name) const
	{
		const auto i = _classes.find(name);
		return i != _classes.end() ? &i->second->root() : nullptr;
	}
}
