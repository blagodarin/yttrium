/// \file
/// \brief Static string.

#ifndef __Y_STATIC_STRING_HPP
#define __Y_STATIC_STRING_HPP

#include <cstring> // strcmp, strlen

#include <Yttrium/safe_bool.hpp>
#include <Yttrium/types.hpp>

namespace Yttrium
{

/// Static string wrapper class.
/// \note A StaticString can't hold \c NULL and, therefore, is always valid.

class StaticString
{
public:

	/// Construct an empty StaticString.

	StaticString() throw()
		: _text(&Null)
		, _size(0)
	{
	}

	/// Construct a StaticString from the C-string.
	/// \param text Source text.

	StaticString(const char *text) throw()
		: _text(text)
		, _size(strlen(text))
	{
	}

	/// Construct a StaticString from the \a text of known \a size.
	/// \param text Source text.
	/// \param size Source text size.

	StaticString(const char *text, size_t size) throw()
		: _text(text)
		, _size(size)
	{
	}

	/// Compares the string with the specified \a string.
	/// \param string The string to compare with.
	/// \return \c strcmp result.

	int compare(const StaticString &string) const throw()
	{
		return strcmp(_text, string._text);
	}

	///

	size_t find_first(char symbol, size_t offset = 0) const throw();

	///

	size_t find_last(char symbol, size_t offset = End) const throw();

	/// Return the string size.
	/// \return String size.

	size_t size() const throw()
	{
		return _size;
	}

	/// Return the string text pointer.
	/// \return String text pointer.

	const char *text() const throw()
	{
		return _text;
	}

public:

	///

	operator SafeBool() const throw()
	{
		return Y_SAFE_BOOL(_size);
	}

	///

	const char &operator [](size_t index) const throw()
	{
		return _text[index];
	}

	///

	bool operator <(const char *string) const throw()
	{
		return (strcmp(_text, string) < 0);
	}

	/**
	* \overload
	*/

	bool operator <(const StaticString &string) const throw()
	{
		return (strcmp(_text, string._text) < 0);
	}

	///

	bool operator >(const char *string) const throw()
	{
		return (strcmp(_text, string) > 0);
	}

	/**
	* \overload
	*/

	bool operator >(const StaticString &string) const throw()
	{
		return (strcmp(_text, string._text) > 0);
	}

	///

	bool operator <=(const char *string) const throw()
	{
		return (strcmp(_text, string) <= 0);
	}

	/**
	* \overload
	*/

	bool operator <=(const StaticString &string) const throw()
	{
		return (strcmp(_text, string._text) <= 0);
	}

	///

	bool operator >=(const char *string) const throw()
	{
		return (strcmp(_text, string) >= 0);
	}

	/**
	* \overload
	*/

	bool operator >=(const StaticString &string) const throw()
	{
		return (strcmp(_text, string._text) >= 0);
	}

	///

	bool operator ==(const char *string) const throw()
	{
		return (strcmp(_text, string) == 0);
	}

	/**
	* \overload
	*/

	bool operator ==(const StaticString &right) const throw()
	{
		return (_size == right._size && strcmp(_text, right._text) == 0);
	}

	///

	bool operator !=(const char *string) const throw()
	{
		return (strcmp(_text, string) != 0);
	}

	/**
	* \overload
	*/

	bool operator !=(const StaticString &string) const throw()
	{
		return (_size != string._size || strcmp(_text, string._text) != 0);
	}

public:

	static const size_t End = SIZE_MAX;

private:

	const char *_text;
	size_t     _size;

private:

	static const char Null = '\0';
};

///

inline bool operator <(const char *left, const StaticString &right) throw()
{
	return (strcmp(left, right.text()) < 0);
}

///

inline bool operator >(const char *left, const StaticString &right) throw()
{
	return (strcmp(left, right.text()) > 0);
}

///

inline bool operator <=(const char *left, const StaticString &right) throw()
{
	return (strcmp(left, right.text()) <= 0);
}

///

inline bool operator >=(const char *left, const StaticString &right) throw()
{
	return (strcmp(left, right.text()) >= 0);
}

///

inline bool operator ==(const char *left, const StaticString &right) throw()
{
	return (strcmp(left, right.text()) == 0);
}

///

inline bool operator !=(const char *left, const StaticString &right) throw()
{
	return (strcmp(left, right.text()) != 0);
}

} // namespace Yttrium

#endif // __Y_STATIC_STRING_HPP
