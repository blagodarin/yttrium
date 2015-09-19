/// \file
/// \brief

#ifndef _include_yttrium_ion_node_h_
#define _include_yttrium_ion_node_h_

#include <yttrium/ion/list.h>
#include <yttrium/string.h>

namespace Yttrium
{
	///
	class Y_API IonNode : public IonList
	{
		friend IonDocumentPrivate;
		friend IonObject;

	public:

		///
		bool exists() const;

		///
		StaticString name() const { return _name; }

	private:

		Y_PRIVATE IonNode(IonDocumentPrivate& document);
		Y_PRIVATE IonNode(IonDocumentPrivate& document, const StaticString& name);
		Y_PRIVATE IonNode(IonDocumentPrivate& document, const StaticString& name, const ByReference&);

	private:

		const String _name;
	};
}

#endif
