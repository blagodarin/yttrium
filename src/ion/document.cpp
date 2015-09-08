#include "document.h"

#include <yttrium/ion/list.h>
#include "../base/file.h"
#include "parser.h"

namespace Yttrium
{
	IonValue* IonDocument::Private::new_list_value()
	{
		return new(_values.allocate()) IonValue(_document);
	}

	IonNode* IonDocument::Private::new_node(const StaticString& name)
	{
		return new(_nodes.allocate()) IonNode(_document, name);
	}

	IonNode* IonDocument::Private::new_node(const StaticString& name, const ByReference&)
	{
		return new(_nodes.allocate()) IonNode(_document, name, ByReference());
	}

	IonObject* IonDocument::Private::new_object()
	{
		return new(_objects.allocate()) IonObject(_document);
	}

	IonValue* IonDocument::Private::new_object_value(IonObject* object)
	{
		return new(_values.allocate()) IonValue(_document, object);
	}

	IonValue* IonDocument::Private::new_value(const StaticString& name)
	{
		return new(_values.allocate()) IonValue(_document, name);
	}

	IonValue* IonDocument::Private::new_value(const StaticString& name, const ByReference&)
	{
		return new(_values.allocate()) IonValue(_document, name, ByReference());
	}

	IonDocument::IonDocument(Allocator* allocator)
		: IonObject(this)
		, _private(Y_NEW(allocator, Private)(this, allocator))
	{
	}

	IonDocument::~IonDocument()
	{
		IonObject::clear();
		_private->_buffer.clear();
		Y_DELETE(_private->_allocator, _private);
	}

	Allocator* IonDocument::allocator() const
	{
		return _private->_allocator;
	}

	bool IonDocument::load(const StaticString& name)
	{
		clear();
		return File(name, _private->_allocator).read_all(&_private->_buffer)
			&& IonParser(this).parse(_private->_buffer, name);
	}

	bool IonDocument::save(const StaticString& name, int indentation) const
	{
		File file(name, File::Write | File::Truncate, _private->_allocator);
		if (!file)
			return false;
		String buffer(_private->_allocator);
		serialize(&buffer, (indentation > 0 ? 0 : indentation), true);
		if (file.write(buffer.text(), buffer.size()) != buffer.size())
			return false;
		return file.flush();
	}
}
