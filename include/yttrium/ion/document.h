/// \file
/// \brief

#ifndef _include_yttrium_ion_document_h_
#define _include_yttrium_ion_document_h_

#include <yttrium/memory/global.h>

#include <memory>

namespace Yttrium
{
	class IonObject;
	class Reader;
	class StaticString;

	///
	class Y_API IonDocument
	{
	public:
		///
		enum class Formatting
		{
			Pretty, ///<
			Compact, ///<
		};

		/// Creates an empty document.
		static std::unique_ptr<IonDocument> create(Allocator& = *DefaultAllocator);

		///
		/// Returns 'nullptr' for a null Reader.
		/// May throw DataError.
		static std::unique_ptr<IonDocument> open(const Reader&, Allocator& = *DefaultAllocator);

		///
		virtual ~IonDocument() = default;

		///
		virtual IonObject& root() = 0;
		virtual const IonObject& root() const = 0;

		///
		virtual bool save(const StaticString& path, Formatting = Formatting::Pretty) const = 0;
	};
}

#endif
