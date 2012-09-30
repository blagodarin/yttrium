#include "classes.h"

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

bool Classes::add(const StaticString &name, const Ion::Object &source, const StaticString &base_class)
{
	if (name.is_empty() || _classes.find(String(name, ByReference())) != _classes.end())
	{
		return false;
	}

	Ion::Object *base = nullptr;

	if (!base_class.is_empty())
	{
		Map::iterator i = _classes.find(String(base_class, ByReference()));
		if (i == _classes.end())
		{
			return false;
		}
		base = &i->second;
	}

	Ion::Document &document = _classes[String(name, _allocator)];

	// Read the object entries in reverse order, ingnoring the duplicates.
	// The class' own order is only significant for the class being added,
	// not its base class.

	update_document(&document, source);

	if (base)
	{
		update_document(&document, *base);
	}

	return true;
}

void Classes::clear()
{
	_classes.clear();
}

const Ion::Object *Classes::find(const StaticString &name) const
{
	Map::const_iterator i = _classes.find(String(name, ByReference()));
	return i != _classes.end() ? &i->second : nullptr;
}

} // namespace Gui

} // namespace Yttrium