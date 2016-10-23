/// \file
/// \brief

#ifndef _include_yttrium_ion_document_h_
#define _include_yttrium_ion_document_h_

#include <yttrium/memory/global.h>
#include <yttrium/resources/resource.h>

namespace Yttrium
{
	class IonObject;
	class Reader;
	template <typename> class ResourcePtr;
	class StaticString;

	///
	class Y_API IonDocument : public Resource
	{
	public:
		///
		enum class Formatting
		{
			Pretty, ///<
			Compact, ///<
		};

		/// Creates an empty document.
		static ResourcePtr<IonDocument> create(Allocator& = *DefaultAllocator);

		///
		static ResourcePtr<IonDocument> open(const Reader&, Allocator& = *DefaultAllocator);

		///
		virtual IonObject& root() = 0;
		virtual const IonObject& root() const = 0;

		///
		virtual bool save(const StaticString& path, Formatting = Formatting::Pretty) const = 0;
	};
}

#endif
