#include "classes.h"

#include <yttrium/allocator.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>

namespace Yttrium
{
	namespace
	{
		void update_document(IonDocument* target, const IonObject& source)
		{
			for (const IonNode& node : source.nodes().reverse())
			{
				if (!target->contains(node.name()))
					target->append(node);
			}
		}
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
			base = i->second;
		}

		IonDocument* document = Y_NEW(_allocator, IonDocument)(_allocator);

		// Read the object entries in reverse order, ingnoring the duplicates.
		// The class' own order is only significant for the class being added,
		// not its base class.

		update_document(document, source);

		if (base)
			update_document(document, *base);

		_classes[String(name, _allocator)] = document;

		return true;
	}

	void GuiClasses::clear()
	{
		for (const auto& class_ : _classes)
			Y_DELETE(_allocator, class_.second);
		_classes.clear();
	}

	const IonObject* GuiClasses::find(const StaticString& name) const
	{
		auto i = _classes.find(String(name, ByReference()));
		return i != _classes.end() ? i->second : nullptr;
	}
}
