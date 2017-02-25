#ifndef _src_ion_document_h_
#define _src_ion_document_h_

#include <yttrium/ion/document.h>

#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/memory/pool.h>

namespace Yttrium
{
	class IonDocumentImpl final : public IonDocument
	{
	public:
		static IonDocumentImpl null;
		static const IonNode null_node;

		IonDocumentImpl(bool exists = true);

		IonObject& root() override { return _root; }
		const IonObject& root() const override { return _root; }
		bool save(const std::string&, Formatting) const override;

		void clear();
		bool exists() const { return _exists; }
		IonValue* new_list_value();
		IonNode* new_node(const StaticString& name);
		IonNode* new_node(const StaticString& name, const ByReference&);
		IonObject* new_object();
		IonValue* new_object_value(IonObject* object);
		IonValue* new_value(const StaticString& text);
		IonValue* new_value(const StaticString& text, const ByReference&);

	private:
		const bool _exists; // TODO: Remove.
		Buffer _buffer;
		IonObject _root;
		Pool<IonObject> _objects;
		Pool<IonNode> _nodes;
		Pool<IonValue> _values;

		friend IonDocument;
		friend IonParser;
	};
}

#endif
