#include "document.h"

#include <yttrium/io/reader.h>
#include <yttrium/io/writer.h>
#include <yttrium/ion/list.h>
#include <yttrium/resources/resource_ptr.h>
#include "parser.h"
#include "utils.h"

namespace Yttrium
{
	IonDocumentImpl IonDocumentImpl::null(NoAllocator);
	const IonNode IonDocumentImpl::null_node(null);

	bool IonDocumentImpl::save(const StaticString& path, Formatting formatting) const
	{
		// TODO: Serialize directly to the writer.
		return Writer(path).write_all(Ion::serialize(_root, true, formatting == Formatting::Pretty ? 0 : -1, &_allocator));
	}

	IonValue* IonDocumentImpl::new_list_value()
	{
		return new(_values.allocate()) IonValue(*this);
	}

	IonNode* IonDocumentImpl::new_node(const StaticString& name)
	{
		return new(_nodes.allocate()) IonNode(*this, name);
	}

	IonNode* IonDocumentImpl::new_node(const StaticString& name, const ByReference&)
	{
		return new(_nodes.allocate()) IonNode(*this, name, ByReference());
	}

	IonObject* IonDocumentImpl::new_object()
	{
		return new(_objects.allocate()) IonObject(*this);
	}

	IonValue* IonDocumentImpl::new_object_value(IonObject* object)
	{
		return new(_values.allocate()) IonValue(*this, object);
	}

	IonValue* IonDocumentImpl::new_value(const StaticString& name)
	{
		return new(_values.allocate()) IonValue(*this, name);
	}

	IonValue* IonDocumentImpl::new_value(const StaticString& name, const ByReference&)
	{
		return new(_values.allocate()) IonValue(*this, name, ByReference());
	}

	bool IonDocumentImpl::load(const Reader& reader)
	{
		return reader.read_all(_buffer) && IonParser(*this).parse(_buffer, reader.name());
	}

	ResourcePtr<IonDocument> IonDocument::create(Allocator& allocator)
	{
		return make_resource<IonDocumentImpl>(allocator);
	}

	ResourcePtr<IonDocument> IonDocument::open(const Reader& reader, Allocator& allocator)
	{
		auto document = make_resource<IonDocumentImpl>(allocator);
		return document->load(reader) ? std::move(document) : nullptr;
	}
}
