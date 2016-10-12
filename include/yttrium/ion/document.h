/// \file
/// \brief

#ifndef _include_yttrium_ion_document_h_
#define _include_yttrium_ion_document_h_

#include <yttrium/memory/unique_ptr.h>

namespace Yttrium
{
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
		IonDocument(Allocator& = *DefaultAllocator);

		///
		Allocator& allocator() const { return _private.allocator(); }

		///
		bool load(const StaticString& path);

		///
		IonObject& root();
		const IonObject& root() const;

		///
		bool save(const StaticString& path, Formatting = Formatting::Pretty) const;

	private:
		UniquePtr<class IonDocumentPrivate> _private;

	public:
		~IonDocument();
		IonDocument(IonDocument&&) noexcept;
		IonDocument& operator=(IonDocument&&) noexcept;
	};
}

#endif
