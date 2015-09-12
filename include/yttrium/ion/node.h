/// \file
/// \brief

#ifndef __Y_ION_NODE_H
#define __Y_ION_NODE_H

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
