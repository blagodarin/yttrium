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
	class Y_API IonDocument: public IonObject
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
		IonDocument(const IonDocument& document, Allocator* allocator = nullptr);

		///
		~IonDocument();

		///
		Allocator* allocator() const;

		///
		void clear();

		///
		bool load(const StaticString& name);

		///
		bool save(const StaticString& name, int indentation = 0) const;

		///
		IonDocument& operator=(const IonDocument& document)
		{
			clear();
			concatenate(document);
			return *this;
		}

	private:
		class Private;
		Private* _private;
	};
}

#endif // __Y_ION_DOCUMENT_H
