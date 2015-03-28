/// \file
/// \brief

#ifndef __Y_ION_NODE_H
#define __Y_ION_NODE_H

#include <yttrium/ion/list.h>
#include <yttrium/string.h>

namespace Yttrium
{

namespace Ion
{

class Document;
class Object;

///

class Y_API Node
	: public List
{
	friend Document;
	friend Object;

public:

	///

	Node();

public:

	///

	inline StaticString name() const { return _name; }

	///

	void serialize(String *result, int indentation = 0) const;

	///

	String serialize(int indentation = 0, Allocator *allocator = nullptr) const;

	///

	StaticString string(const StaticString &default_value = StaticString()) const;

private:

	Y_PRIVATE Node(Document *document, const StaticString &name);
	Y_PRIVATE Node(Document *document, const StaticString &name, const ByReference &);

private:

	String _name;
};

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_NODE_H
