#ifndef _src_gui_ion_classes_h_
#define _src_gui_ion_classes_h_

#include <yttrium/std/map.h>

namespace Yttrium
{
	class IonDocument;
	class IonObject;
	template <typename> class ResourcePtr;
	class StaticString;
	class String;

	class GuiClasses
	{
	public:
		GuiClasses(Allocator&);
		~GuiClasses();

		bool add(const StaticString& name, const IonObject& source, const StaticString* base_class = nullptr);
		void clear();
		const IonObject* find(const StaticString& name) const;

	private:
		Allocator& _allocator;
		StdMap<String, ResourcePtr<IonDocument>> _classes;
	};
}

#endif
