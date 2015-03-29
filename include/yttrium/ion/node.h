/// \file
/// \brief

#ifndef __Y_ION_NODE_H
#define __Y_ION_NODE_H

#include <yttrium/ion/list.h>
#include <yttrium/string.h>

namespace Yttrium
{
	class IonDocument;
	class IonObject;

	///
	class Y_API IonNode: public IonList
	{
		friend IonDocument;
		friend IonObject;

	public:

		///
		IonNode();

		///
		StaticString name() const { return _name; }

		///
		void serialize(String* result, int indentation = 0) const;

		///
		String serialize(int indentation = 0, Allocator* allocator = nullptr) const;

		///
		StaticString string(const StaticString& default_value = StaticString()) const;

	private:

		Y_PRIVATE IonNode(IonDocument* document, const StaticString& name);
		Y_PRIVATE IonNode(IonDocument* document, const StaticString& name, const ByReference&);

	private:

		String _name;
	};
}

#endif // __Y_ION_NODE_H
