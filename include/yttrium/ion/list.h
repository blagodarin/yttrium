/// \file
/// \brief

#ifndef __Y_ION_LIST_H
#define __Y_ION_LIST_H

#include <yttrium/global.h>

#include <cstddef>

namespace Yttrium
{
	class Allocator;
	class ByReference;
	class IonDocument;
	class IonObject;
	class IonParser;
	class IonValue;
	class StaticString;
	class String;

	///
	class Y_API IonList
	{
		friend IonParser;
		friend IonValue;

	public:

		///
		class ConstIterator
		{
			friend IonList;

		public:

			void operator++();
			const IonValue& operator*() const { return *_value; }
			bool operator!=(ConstIterator iterator) const { return _value != iterator._value; }

		private:

			const IonValue* _value;

			ConstIterator(const IonValue* value): _value(value) {}
		};

		///
		class ConstRange
		{
			friend IonList;

		public:

			ConstRange() = default;

			const IonValue& first() const { return *_first; }
			bool is_empty() const { return !_size; }
			const IonValue& last() const { return *_last; }
			void pop_first();
			void pop_last();
			size_t size() const { return _size; }

			const IonValue* operator ->() const { return _first; }

		private:

			const IonValue* _first = nullptr;
			const IonValue* _last = nullptr;
			size_t          _size = 0;

			ConstRange(const IonValue* first, const IonValue* last, size_t size): _first(first), _last(last), _size(size) {}
		};

		///
		IonList* append_list();

		///
		IonList* append_list(const IonList& list);

		///
		IonObject* append_object();

		///
		IonObject* append_object(const IonObject& object);

		///
		IonValue* append(const StaticString& string);

		///
		ConstIterator begin() const { return ConstIterator(_first); }

		///
		void concatenate(const IonList& list);

		///
		const IonDocument* document() const { return _document; }

		///
		ConstIterator end() const { return ConstIterator(nullptr); }

		///
		bool exists() const { return _document; }

		///
		const IonValue* first() const { return _first; }

		///
		bool first(const StaticString** value) const;

		///
		bool is_empty() const { return !_size; }

		///
		const IonValue* last() const { return _last; }

		///
		void serialize(String* result, int indentation = 0) const { serialize(result, indentation, false); }

		///
		String serialize(int indentation = 0, Allocator* allocator = nullptr) const;

		///
		size_t size() const { return _size; }

		///
		ConstRange values() const { return ConstRange(_first, _last, _size); }

		IonList(const IonList&) = delete;
		IonList& operator=(const IonList&) = delete;

	protected:

		IonList(IonDocument* document): _document(document) {}

		Y_PRIVATE void serialize(String* result, int indentation, bool is_node) const;

	private:

		Y_PRIVATE IonValue* append(const StaticString& string, const ByReference&);
		Y_PRIVATE void append(IonValue* value);

	private:

		IonDocument* _document;
		IonValue*    _first = nullptr;
		IonValue*    _last = nullptr;
		size_t       _size = 0;
	};
}

#endif
