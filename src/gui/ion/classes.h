#ifndef __GUI_ION_CLASSES_H
#define __GUI_ION_CLASSES_H

#include <Yttrium/ion/document.h>

#include <map>

namespace Yttrium
{

namespace Gui
{

class Classes
{
public:

	inline Classes(Allocator *allocator = DefaultAllocator);

public:

	bool add(const StaticString &name, const Ion::Object &source,
		const StaticString *base_class = nullptr);

	void clear();

	const Ion::Object *find(const StaticString &name) const;

private:

	typedef std::map<String, Ion::Document> Map;

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

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_ION_CLASSES_H
