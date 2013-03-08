/// \file
/// \brief

#ifndef __Y_ION_DOCUMENT_H
#define __Y_ION_DOCUMENT_H

#include <Yttrium/ion/object.h>

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

	Document(Allocator *allocator = DefaultAllocator) noexcept;

	///

	Document(const Document &document, Allocator *allocator = nullptr) noexcept;

	///

	~Document() noexcept;

public:

	///

	Allocator *allocator() const noexcept;

	///

	void clear() noexcept;

	///

	bool load(const StaticString &name) noexcept;

	///

	void save(const StaticString &name, int indentation = 0) const noexcept;

public:

	///

	inline Document &operator =(const Document &document) noexcept;

private:

	class Private;

	Private *_private;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Document &Document::operator =(const Document &document) noexcept
{
	clear();
	concatenate(document);
	return *this;
}

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_DOCUMENT_H
