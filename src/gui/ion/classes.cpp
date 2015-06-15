#include "classes.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>

namespace Yttrium
{
	namespace
	{
		void update_document(IonDocument& document, const IonObject& source)
		{
			for (const IonNode& node : source.nodes().reverse())
			{
				if (!document.contains(node.name()))
					document.append(node);
			}
		}
	}

	GuiClasses::GuiClasses(Allocator* allocator)
		: _allocator(allocator)
	{
	}

	GuiClasses::~GuiClasses()
	{
	}

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
			base = i->second.get();
		}

		auto document = make_pointer<IonDocument>(*_allocator, _allocator);
		update_document(*document, source);
		if (base)
			update_document(*document, *base);

		_classes.emplace(String(name, _allocator), std::move(document));

		return true;
	}

	void GuiClasses::clear()
	{
		_classes.clear();
	}

	const IonObject* GuiClasses::find(const StaticString& name) const
	{
		auto i = _classes.find(String(name, ByReference()));
		return i != _classes.end() ? i->second.get() : nullptr;
	}
}
