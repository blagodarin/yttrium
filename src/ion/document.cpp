#include "document.h"

#include <yttrium/exceptions.h>
#include <yttrium/ion/list.h>
#include <yttrium/storage/source.h>
#include "parser.h"

namespace Yttrium
{
	IonDocumentImpl IonDocumentImpl::null(false);
	const IonNode IonDocumentImpl::null_node(null);

	IonDocumentImpl::IonDocumentImpl(bool exists)
		: _exists{exists}
		, _root{*this}
		, _objects{32}
		, _nodes{32}
		, _values{32}
	{
	}

	IonValue* IonDocumentImpl::new_list_value()
	{
		return new(_values.allocate()) IonValue{*this};
	}

	IonNode* IonDocumentImpl::new_node(std::string_view name)
	{
		return new(_nodes.allocate()) IonNode{*this, name};
	}

	IonNode* IonDocumentImpl::new_node(std::string_view name, const ByReference&)
	{
		return new(_nodes.allocate()) IonNode{*this, name, ByReference{}};
	}

	IonObject* IonDocumentImpl::new_object()
	{
		return new(_objects.allocate()) IonObject{*this};
	}

	IonValue* IonDocumentImpl::new_object_value(IonObject* object)
	{
		return new(_values.allocate()) IonValue{*this, object};
	}

	IonValue* IonDocumentImpl::new_value(std::string_view text)
	{
		return new(_values.allocate()) IonValue{*this, text};
	}

	IonValue* IonDocumentImpl::new_value(std::string_view text, const ByReference&)
	{
		return new(_values.allocate()) IonValue{*this, text, ByReference{}};
	}

	std::unique_ptr<IonDocument> IonDocument::create()
	{
		return std::make_unique<IonDocumentImpl>();
	}

	std::unique_ptr<IonDocument> IonDocument::load(const Source& source)
	{
		auto document = std::make_unique<IonDocumentImpl>();
		document->_buffer = source.to_buffer();
		const auto parsing_result = IonParser::parse(*document);
		if (parsing_result.status != IonParser::Status::Ok)
			throw DataError{'(', source.name(), ':', parsing_result.line, ':', parsing_result.position, ") Syntax error"};
		return std::move(document);
	}
}
