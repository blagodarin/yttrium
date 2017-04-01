/// \file
/// \brief

#ifndef _include_yttrium_ion_document_h_
#define _include_yttrium_ion_document_h_

#include <yttrium/api.h>

#include <memory>
#include <string>

namespace Yttrium
{
	class IonObject;
	class Source;

	///
	class Y_API IonDocument
	{
	public:
		/// Creates an empty document.
		static std::unique_ptr<IonDocument> create();

		///
		/// May throw DataError.
		static std::unique_ptr<IonDocument> load(const Source&);

		///
		virtual ~IonDocument() = default;

		///
		virtual IonObject& root() = 0;
		virtual const IonObject& root() const = 0;
	};
}

#endif
