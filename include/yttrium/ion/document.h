/// \file
/// \brief

#ifndef __Y_ION_DOCUMENT_H
#define __Y_ION_DOCUMENT_H

#include <yttrium/ion/object.h>

namespace Yttrium
{

namespace Ion
{

class List;

///
class Y_API Document: public Object
{
	friend List;
	friend Node;
	friend Object;
	friend Parser;
	friend Value;

public:

	///
	Document(Allocator* allocator = DefaultAllocator) noexcept;

	///
	Document(const Document& document, Allocator* allocator = nullptr) noexcept;

	///
	~Document() noexcept;

public:

	///
	Allocator* allocator() const noexcept;

	///
	void clear() noexcept;

	///
	bool load(const StaticString& name) noexcept;

	///
	bool save(const StaticString& name, int indentation = 0) const noexcept;

public:

	///
	Document& operator=(const Document& document) noexcept
	{
		clear();
		concatenate(document);
		return *this;
	}

private:

	class Private;

	Private* _private;
};

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_DOCUMENT_H
