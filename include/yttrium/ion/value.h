/// \file
/// \brief

#ifndef _include_yttrium_ion_value_h_
#define _include_yttrium_ion_value_h_

#include <yttrium/ion/list.h>
#include <yttrium/string.h>

namespace Yttrium
{
	///
	class Y_API IonValue
	{
		friend IonDocumentPrivate;
		friend IonList;
		friend IonListIterator;
		friend IonListRange;

	public:

		///
		enum class Type
		{
			String, ///<
			List,   ///<
			Object, ///<
		};

		///
		bool get(const StaticString** value) const
		{
			if (_type != Type::String)
				return false;
			*value = &_string;
			return true;
		}

		/**
		* \overload
		*/
		bool get(int32_t* value) const { return _type == Type::String && _string.to_number(*value); }

		/**
		* \overload
		*/
		bool get(float* value) const { return _type == Type::String && _string.to_number(*value); }

		/**
		* \overload
		*/

		bool get(const IonList** value) const
		{
			if (_type != Type::List)
				return false;
			*value = &_list; // NOTE: Something's wrong here.
			return true;
		}

		/**
		* \overload
		*/
		bool get(const IonObject** value) const
		{
			if (_type != Type::Object)
				return false;
			*value = _object;
			return true;
		}

		///
		IonList& list() { return _list; }

		///
		const IonList& list() const { return _list; }

		///
		const StaticString& string() const { return _string; }

		///
		Type type() const { return _type; }

		///
		const IonObject* object() const { return _object; }

		IonValue(const IonValue&) = delete;
		IonValue& operator=(const IonValue&) = delete;

	private:

		Y_PRIVATE IonValue(IonDocumentPrivate&);
		Y_PRIVATE IonValue(IonDocumentPrivate&, const StaticString& string);
		Y_PRIVATE IonValue(IonDocumentPrivate&, const StaticString& string, const ByReference&);
		Y_PRIVATE IonValue(IonDocumentPrivate&, IonObject* object);

	private:

		const Type _type;
		String _string;
		IonList _list;
		IonObject* const _object = nullptr;
		IonValue* _next = nullptr;
		IonValue* _previous = nullptr;
	};
}

#endif
