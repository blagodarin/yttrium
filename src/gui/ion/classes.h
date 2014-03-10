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

class GuiClasses
{
public:

	GuiClasses(Allocator* allocator)
		: _allocator(allocator)
	{
	}

	~GuiClasses()
	{
		clear();
	}

public:

	bool add(const StaticString& name, const Ion::Object& source, const StaticString* base_class = nullptr);
	void clear();
	const Ion::Object* find(const StaticString& name) const;

private:

	Allocator*                       _allocator;
	std::map<String, Ion::Document*> _classes;
};

} // namespace Yttrium

#endif // __GUI_ION_CLASSES_H
