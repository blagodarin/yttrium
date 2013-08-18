#include "document.h"

#include <yttrium/ion/list.h>

#include "../base/file.h"
#include "parser.h"

namespace Yttrium
{

namespace Ion
{

Value *Document::Private::new_list_value()
{
	return new(_values.allocate()) Value(_document);
}

Node *Document::Private::new_node(const StaticString &name)
{
	return new(_nodes.allocate()) Node(_document, name);
}

Node *Document::Private::new_node(const StaticString &name, const ByReference &)
{
	return new(_nodes.allocate()) Node(_document, name, ByReference());
}

Object *Document::Private::new_object()
{
	return new(_objects.allocate()) Object(_document);
}

Value *Document::Private::new_object_value(Object *object)
{
	return new(_values.allocate()) Value(_document, object);
}

Value *Document::Private::new_value(const StaticString &name)
{
	return new(_values.allocate()) Value(_document, name);
}

Value *Document::Private::new_value(const StaticString &name, const ByReference &)
{
	return new(_values.allocate()) Value(_document, name, ByReference());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Document::Document(Allocator *allocator)
	: Object(this)
	, _private(Y_NEW(allocator, Private)(this, allocator))
{
}

Document::Document(const Document &document, Allocator *allocator)
	//: Document(allocator ? allocator : document._allocator) // TODO: Uncomment.
	: Object(this)
{
	if (!allocator)
		allocator = document._private->_allocator;
	_private = Y_NEW(allocator, Private)(this, allocator);
	concatenate(document);
}

Document::~Document()
{
	clear();
	Y_DELETE(_private->_allocator, _private);
}

Allocator *Document::allocator() const
{
	return _private->_allocator;
}

void Document::clear()
{
	Object::clear();
	_private->_buffer.clear();
}

bool Document::load(const StaticString &name)
{
	clear();
	return File(name, _private->_allocator).read_all(&_private->_buffer)
		&& Parser(this).parse(_private->_buffer, name);
}

void Document::save(const StaticString &name, int indentation) const
{
	StaticFile file(name, File::Write | File::Truncate, _private->_allocator);

	if (!file.is_opened())
	{
		// TODO: Log the situation.
	}
	else
	{
		String buffer(_private->_allocator);

		serialize(&buffer, (indentation > 0 ? 0 : indentation), true);

		file.write(buffer.text(), buffer.size());
	}
}

} // namespace Ion

} // namespace Yttrium
