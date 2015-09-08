/// \file
/// \brief

#ifndef __Y_ION_DOCUMENT_H
#define __Y_ION_DOCUMENT_H

#include <yttrium/ion/object.h>
#include <yttrium/types.h>

namespace Yttrium
{
	class IonList;

	///
	class Y_API IonDocument : public IonObject
	{
		friend IonList;
		friend IonNode;
		friend IonObject;
		friend IonParser;
		friend IonValue;

	public:

		///
		IonDocument(Allocator* allocator = DefaultAllocator);

		///
		~IonDocument();

		///
		Allocator* allocator() const;

		///
		bool load(const StaticString& name);

		///
		bool save(const StaticString& name, int indentation = 0) const;

	private:
		class Private;
		Private* _private;
	};
}

#endif
