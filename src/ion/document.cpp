#include "document.h"

#include <yttrium/io/reader.h>
#include <yttrium/io/writer.h>
#include <yttrium/ion/list.h>
#include "parser.h"
#include "utils.h"

namespace Yttrium
{
	IonDocumentPrivate IonDocumentPrivate::null(NoAllocator);
	const IonNode IonDocumentPrivate::null_node(null);

	void IonDocumentPrivate::clear()
	{
		_root.clear();
	}

	IonValue* IonDocumentPrivate::new_list_value()
	{
		return new(_values.allocate()) IonValue(*this);
	}

	IonNode* IonDocumentPrivate::new_node(const StaticString& name)
	{
		return new(_nodes.allocate()) IonNode(*this, name);
	}

	IonNode* IonDocumentPrivate::new_node(const StaticString& name, const ByReference&)
	{
		return new(_nodes.allocate()) IonNode(*this, name, ByReference());
	}

	IonObject* IonDocumentPrivate::new_object()
	{
		return new(_objects.allocate()) IonObject(*this);
	}

	IonValue* IonDocumentPrivate::new_object_value(IonObject* object)
	{
		return new(_values.allocate()) IonValue(*this, object);
	}

	IonValue* IonDocumentPrivate::new_value(const StaticString& name)
	{
		return new(_values.allocate()) IonValue(*this, name);
	}

	IonValue* IonDocumentPrivate::new_value(const StaticString& name, const ByReference&)
	{
		return new(_values.allocate()) IonValue(*this, name, ByReference());
	}

	IonDocument::IonDocument(Allocator& allocator)
		: _private(make_unique<IonDocumentPrivate>(allocator, allocator))
	{
	}

	bool IonDocument::load(const StaticString& path)
	{
		_private->clear();
		return Reader(path).read_all(_private->_buffer) && IonParser(*this).parse(_private->_buffer, path);
	}

	IonObject& IonDocument::root()
	{
		return _private->_root;
	}

	const IonObject& IonDocument::root() const
	{
		return _private->_root;
	}

	bool IonDocument::save(const StaticString& path, Formatting formatting) const
	{
		// TODO: Serialize directly to the writer.
		return Writer(path).write_all(Ion::serialize(_private->_root, true, formatting == Formatting::Pretty ? 0 : -1, &_private->_allocator));
	}

	IonDocument::~IonDocument() = default;
	IonDocument::IonDocument(IonDocument&&) noexcept = default;
	IonDocument& IonDocument::operator=(IonDocument&&) noexcept = default;
}
