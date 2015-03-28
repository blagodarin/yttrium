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
	Document(Allocator* allocator = DefaultAllocator);

	///
	Document(const Document& document, Allocator* allocator = nullptr);

	///
	~Document();

public:

	///
	Allocator* allocator() const;

	///
	void clear();

	///
	bool load(const StaticString& name);

	///
	bool save(const StaticString& name, int indentation = 0) const;

public:

	///
	Document& operator=(const Document& document)
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
