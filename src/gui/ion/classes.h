#ifndef _src_gui_ion_classes_h_
#define _src_gui_ion_classes_h_

#include <memory>
#include <string>
#include <unordered_map>

namespace Yttrium
{
	class IonDocument;
	class IonObject;

	class GuiClasses
	{
	public:
		~GuiClasses();

		bool add(const std::string& name, const IonObject& source, const std::string* base_class = nullptr);
		void clear();
		const IonObject* find(const std::string& name) const;

	private:
		std::unordered_map<std::string, std::unique_ptr<const IonDocument>> _classes;
	};
}

#endif
