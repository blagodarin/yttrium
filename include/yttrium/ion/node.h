#ifndef _include_yttrium_ion_node_h_
#define _include_yttrium_ion_node_h_

#include <yttrium/ion/list.h>
#include <yttrium/string.h>

namespace Yttrium
{
	///
	class Y_API IonNode : public IonList
	{
		friend IonDocumentImpl;
		friend IonObject;

	public:
		///
		bool exists() const;

		///
		std::string_view name() const noexcept { return _name; }

	private:
		Y_PRIVATE explicit IonNode(IonDocumentImpl&);
		Y_PRIVATE IonNode(IonDocumentImpl&, std::string_view);
		Y_PRIVATE IonNode(IonDocumentImpl&, std::string_view, const ByReference&);

	private:
		const String _name;
	};
}

#endif
