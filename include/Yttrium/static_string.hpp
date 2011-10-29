/// \file
/// \brief Static string.
/// \note The idea behind the static string with length is that the length of the string is almost always known.

#ifndef __Y_STATIC_STRING_HPP
#define __Y_STATIC_STRING_HPP

#include <cstring> // strcmp, strlen

#include <Yttrium/safe_bool.hpp>
#include <Yttrium/types.hpp>
#include <Yttrium/utils.hpp>

// We don't need const char * operations since we can convert the vast majority of strings
// (even the literal ones) to StaticStrings of a specified length.

// NOTE: There is no fun in having to write static string algorithms in two variants
// (for static and dynamic strings), so we should do something about it.

namespace Yttrium
{

/// Static string wrapper class.
/// \note Static strings can't implicitly hold \c NULL and therefore should always be valid.
/// \note Static strings may have no zero terminator.

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

public:

	/// Compares the string with the specified \a string.
	/// \param string The string to compare with.
	/// \return \c memcmp result.

	int compare(const StaticString &string) const throw();

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

	bool operator <(const StaticString &string) const throw()
	{
		return (compare(string) < 0);
	}

	///

	bool operator >(const StaticString &string) const throw()
	{
		return (compare(string) > 0);
	}

	///

	bool operator <=(const StaticString &string) const throw()
	{
		return (compare(string) <= 0);
	}

	///

	bool operator >=(const StaticString &string) const throw()
	{
		return (compare(string) >= 0);
	}

	///

	bool operator ==(const StaticString &string) const throw()
	{
		return (_size == string._size && compare(string) == 0);
	}

	///

	bool operator !=(const StaticString &string) const throw()
	{
		return (_size != string._size || compare(string) != 0);
	}

public:

	///

	static const size_t End = SIZE_MAX;

protected:

	const char *_text;
	size_t      _size;

private:

	static const char Null = '\0';
};

} // namespace Yttrium

///

#define Y_S(literal) Yttrium::StaticString(literal, sizeof(literal) - 1)

// NOTE: GCC 4.7 required.

//Yttrium::StaticString operator "" _y(const char *text, size_t size) throw()
//{
//	return StaticString(text, size);
//}

#endif // __Y_STATIC_STRING_HPP
