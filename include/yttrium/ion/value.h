/// \file
/// \brief

#ifndef __Y_ION_VALUE_H
#define __Y_ION_VALUE_H

#include <yttrium/ion/list.h>
#include <yttrium/string.h>

namespace Yttrium
{
	class IonDocument;

	///
	class Y_API IonValue
	{
		friend IonDocument;
		friend IonList;

		Y_NONCOPYABLE(IonValue);

	public:

		///
		enum Type
		{
			StringType, ///<
			ListType,   ///<
			ObjectType, ///<
		};

		///
		bool get(const StaticString** value) const
		{
			if (_type != StringType)
				return false;
			*value = &_string;
			return true;
		}

		/**
		* \overload
		*/
		bool get(int32_t* value) const { return _type == StringType && _string.to_number(value); }

		/**
		* \overload
		*/
		bool get(float* value) const { return _type == StringType && _string.to_number(value); }

		/**
		* \overload
		*/

		bool get(const IonList** value) const
		{
			if (_type != ListType)
				return false;
			*value = &_list; // NOTE: Something's wrong here.
			return true;
		}

		/**
		* \overload
		*/
		bool get(const IonObject** value) const
		{
			if (_type != ObjectType)
				return false;
			*value = _object;
			return true;
		}

		///
		bool is_list() const { return _type == ListType; }

		///
		bool is_object() const { return _type == ObjectType; }

		///
		bool is_string() const { return _type == StringType; }

		///
		IonList& list() { return _list; }

		///
		const IonList& list() const { return _list; }

		///
		void serialize(String* result, int indentation = 0) const;

		///
		String serialize(int indentation = 0, Allocator* allocator = nullptr) const;

		///
		const StaticString& string() const { return _string; }

		///
		Type type() const { return _type; }

		///
		const IonObject* object() const { return _object; }

	private:

		Y_PRIVATE IonValue(IonDocument* document);
		Y_PRIVATE IonValue(IonDocument* document, const StaticString& string);
		Y_PRIVATE IonValue(IonDocument* document, const StaticString& string, const ByReference&);
		Y_PRIVATE IonValue(IonDocument* document, IonObject* object);

	private:

		Type       _type;
		String     _string;
		IonList    _list;
		IonObject* _object;
		IonValue*  _next;
		IonValue*  _previous;
	};
}

#endif // __Y_ION_VALUE_H
