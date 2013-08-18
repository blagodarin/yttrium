#ifndef __GUI_ION_CLASSES_H
#define __GUI_ION_CLASSES_H

#include <yttrium/static_string.h>

#include <map>

namespace Yttrium
{

namespace Ion
{

class Document;
class Object;

} // namespace Ion

namespace Gui
{

class Classes
{
public:

	inline Classes(Allocator *allocator);

	inline ~Classes();

public:

	bool add(const StaticString &name, const Ion::Object &source,
		const StaticString *base_class = nullptr);

	void clear();

	const Ion::Object *find(const StaticString &name) const;

private:

	typedef std::map<String, Ion::Document*> Map;

private:

	Allocator *_allocator;
	Map        _classes;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Classes::Classes(Allocator *allocator)
	: _allocator(allocator)
{
}

Classes::~Classes()
{
	clear();
}

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_ION_CLASSES_H
