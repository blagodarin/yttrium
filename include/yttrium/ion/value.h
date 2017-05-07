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
		friend IonDocumentImpl;
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
		bool get(std::string_view& value) const
		{
			if (_type != Type::String)
				return false;
			value = _string;
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
		std::string_view string() const { return _string; }

		///
		Type type() const { return _type; }

		///
		const IonObject* object() const { return _object; }

		IonValue(const IonValue&) = delete;
		IonValue& operator=(const IonValue&) = delete;

	private:
		Y_PRIVATE explicit IonValue(IonDocumentImpl&);
		Y_PRIVATE IonValue(IonDocumentImpl&, std::string_view);
		Y_PRIVATE IonValue(IonDocumentImpl&, std::string_view, const ByReference&);
		Y_PRIVATE IonValue(IonDocumentImpl&, IonObject*);

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
