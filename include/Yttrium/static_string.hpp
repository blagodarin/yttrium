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
		: _text(&null)
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

	/// String comparison operator.

	bool operator <(const StaticString &string) const throw()
	{
		return strcmp(_text, string._text) < 0;
	}

	/// String comparison operator.

	bool operator >(const StaticString &string) const throw()
	{
		return strcmp(_text, string._text) > 0;
	}

	/// String comparison operator.

	bool operator <=(const StaticString &string) const throw()
	{
		return strcmp(_text, string._text) <= 0;
	}

	/// String comparison operator.

	bool operator >=(const StaticString &string) const throw()
	{
		return strcmp(_text, string._text) >= 0;
	}

	/// String comparison operator.

	bool operator ==(const StaticString &string) const throw()
	{
		return strcmp(_text, string._text) == 0;
	}

	/// String comparison operator.

	bool operator !=(const StaticString &string) const throw()
	{
		return strcmp(_text, string._text) != 0;
	}

protected:

	///

	static const char null = '\0';

private:

	const char *_text;
	size_t     _size;
};

} // namespace Yttrium

#endif // __Y_STATIC_STRING_HPP
