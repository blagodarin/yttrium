#include "classes.h"

#include <Yttrium/allocator.h>
#include <Yttrium/ion/document.h>
#include <Yttrium/ion/node.h>

namespace Yttrium
{

namespace Gui
{

namespace
{

void update_document(Ion::Document *target, const Ion::Object &source)
{
	for (Ion::Object::ConstRange r = source.nodes(); !r.is_empty(); r.pop_last())
	{
		const Ion::Node &node = r.last();
		if (!target->contains(node.name()))
		{
			target->append(node);
		}
	}
}

} // namespace

bool Classes::add(const StaticString &name, const Ion::Object &source, const StaticString *base_class)
{
	if (name.is_empty() || _classes.find(String(name, ByReference())) != _classes.end())
	{
		return false;
	}

	Ion::Object *base = nullptr;

	if (base_class)
	{
		Map::iterator i = _classes.find(String(*base_class, ByReference()));
		if (i == _classes.end())
		{
			return false;
		}
		base = i->second;
	}

	Ion::Document *document = Y_NEW(_allocator, Ion::Document)(_allocator);

	// Read the object entries in reverse order, ingnoring the duplicates.
	// The class' own order is only significant for the class being added,
	// not its base class.

	update_document(document, source);

	if (base)
	{
		update_document(document, *base);
	}

	_classes[String(name, _allocator)] = document;

	return true;
}

void Classes::clear()
{
	for (const auto &class_: _classes)
	{
		Y_DELETE(_allocator, class_.second);
	}

	_classes.clear();
}

const Ion::Object *Classes::find(const StaticString &name) const
{
	Map::const_iterator i = _classes.find(String(name, ByReference()));
	return i != _classes.end() ? i->second : nullptr;
}

} // namespace Gui

} // namespace Yttrium
