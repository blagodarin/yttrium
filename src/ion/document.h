#ifndef __ION_DOCUMENT_H
#define __ION_DOCUMENT_H

#include <yttrium/ion/document.h>

#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/pool.h>

namespace Yttrium
{
	class IonDocumentPrivate
	{
		friend IonDocument;

	public:

		static IonDocumentPrivate null;
		static const IonNode null_node;

		IonDocumentPrivate(Allocator* allocator)
			: _allocator(allocator)
			, _buffer(allocator)
			, _root(*this)
			, _objects(32, allocator)
			, _nodes(32, allocator)
			, _values(32, allocator)
		{
			// TODO: Get rid of the magic numbers above.
		}

		Allocator* allocator() const { return _allocator; }
		void clear();
		IonValue* new_list_value();
		IonNode* new_node(const StaticString& name);
		IonNode* new_node(const StaticString& name, const ByReference&);
		IonObject* new_object();
		IonValue* new_object_value(IonObject* object);
		IonValue* new_value(const StaticString& text);
		IonValue* new_value(const StaticString& name, const ByReference&);

	private:

		Allocator* const _allocator;
		String _buffer;
		IonObject _root;
		Pool<IonObject> _objects;
		Pool<IonNode> _nodes;
		Pool<IonValue> _values;
	};
}

#endif
