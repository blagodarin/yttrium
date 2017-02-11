#ifndef _src_gui_ion_classes_h_
#define _src_gui_ion_classes_h_

#include <yttrium/resources/resource_ptr.h>

#include <string>
#include <unordered_map>

namespace Yttrium
{
	class Allocator;
	class IonDocument;
	class IonObject;

	class GuiClasses
	{
	public:
		GuiClasses(Allocator&);
		~GuiClasses();

		bool add(const std::string& name, const IonObject& source, const std::string* base_class = nullptr);
		void clear();
		const IonObject* find(const std::string& name) const;

	private:
		Allocator& _allocator;
		std::unordered_map<std::string, ResourcePtr<const IonDocument>> _classes;
	};
}

#endif
