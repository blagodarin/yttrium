#ifndef __GUI_ION_CLASSES_H
#define __GUI_ION_CLASSES_H

#include <yttrium/pointer.h>

#include <map>

namespace Yttrium
{
	class Allocator;
	class IonDocument;
	class IonObject;
	class StaticString;
	class String;

	class GuiClasses
	{
	public:

		GuiClasses(Allocator* allocator);
		~GuiClasses();

		bool add(const StaticString& name, const IonObject& source, const StaticString* base_class = nullptr);
		void clear();
		const IonObject* find(const StaticString& name) const;

	private:

		Allocator* _allocator;
		std::map<String, Pointer<IonDocument>> _classes;
	};
}

#endif // __GUI_ION_CLASSES_H
