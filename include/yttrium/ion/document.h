/// \file
/// \brief

#ifndef _include_yttrium_ion_document_h_
#define _include_yttrium_ion_document_h_

#include <yttrium/types.h>

namespace Yttrium
{
	class IonDocumentPrivate;
	class IonObject;
	class StaticString;

	///
	class Y_API IonDocument
	{
	public:

		///
		enum class Formatting
		{
			Pretty,  ///<
			Compact, ///<
		};

		///
		IonDocument(Allocator* allocator = DefaultAllocator);

		///
		Allocator* allocator() const;

		///
		bool load(const StaticString& file_name);

		///
		IonObject& root();
		const IonObject& root() const;

		///
		bool save(const StaticString& file_name, Formatting formatting = Formatting::Pretty) const;

		IonDocument(const IonDocument&) = delete;
		IonDocument(IonDocument&& document): _private(document._private) { document._private = nullptr; }
		~IonDocument();
		IonDocument& operator=(const IonDocument&) = delete;
		IonDocument& operator=(IonDocument&&);
	private:
		IonDocumentPrivate* _private = nullptr;
	};
}

#endif
