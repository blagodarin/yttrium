#include "document.h"

#include <yttrium/exceptions.h>
#include <yttrium/ion/list.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>
#include "parser.h"
#include "utils.h"

namespace Yttrium
{
	IonDocumentImpl IonDocumentImpl::null(false);
	const IonNode IonDocumentImpl::null_node(null);

	IonDocumentImpl::IonDocumentImpl(bool exists)
		: _exists{ exists }
		, _root{ *this }
		, _objects{ 32 }
		, _nodes{ 32 }
		, _values{ 32 }
	{
	}

	bool IonDocumentImpl::save(const std::string& path, Formatting formatting) const
	{
		return Writer(path).write_all(StaticString{ Ion::serialize(_root, true, formatting == Formatting::Pretty ? 0 : -1) });
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

	IonValue* IonDocumentImpl::new_value(const StaticString& text)
	{
		return new(_values.allocate()) IonValue(*this, text);
	}

	IonValue* IonDocumentImpl::new_value(const StaticString& text, const ByReference&)
	{
		return new(_values.allocate()) IonValue(*this, text, ByReference());
	}

	std::unique_ptr<IonDocument> IonDocument::create()
	{
		return std::make_unique<IonDocumentImpl>();
	}

	std::unique_ptr<IonDocument> IonDocument::open(const Reader& reader)
	{
		auto document = std::make_unique<IonDocumentImpl>();
		if (!reader.read_all(document->_buffer))
			return nullptr;
		const auto parsing_result = IonParser::parse(*document);
		if (parsing_result.status != IonParser::Status::Ok)
			throw DataError('(', reader.name(), ':', parsing_result.line, ':', parsing_result.position, ") Syntax error"_s);
		return std::move(document);
	}
}
