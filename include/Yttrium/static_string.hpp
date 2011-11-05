/// \file
/// \brief Static string.
/// \note The idea behind the static string with length is that the length of the string is almost always known.

#ifndef __Y_STATIC_STRING_HPP
#define __Y_STATIC_STRING_HPP

#include <cstring> // memcmp, strlen

#include <Yttrium/safe_bool.hpp>
#include <Yttrium/types.hpp>
#include <Yttrium/utils.hpp>

// We don't need const char * operations since we can convert the vast majority of strings
// (even the literal ones) to StaticStrings of a specified length.

namespace Yttrium
{

/// Static string wrapper class.
/// \note Static strings can't implicitly hold \c NULL and therefore should always be valid.
/// \note Static strings may have no zero terminator.

class Y_API StaticString
{
	friend class String;

public:

	/// Construct an empty StaticString.

	StaticString() throw()
		: _text(const_cast<char *>(&Null))
		, _size(0)
	{
	}

	/// Construct a StaticString from the C-string.
	/// \param text Source text.

	StaticString(const char *text) throw()
		: _text(const_cast<char *>(text))
		, _size(strlen(text))
	{
	}

	/// Construct a StaticString from the \a text of known \a size.
	/// \param text Source text.
	/// \param size Source text size.

	StaticString(const char *text, size_t size) throw()
		: _text(const_cast<char *>(text))
		, _size(size)
	{
	}

public:

	/// Compares the string with the specified \a string.
	/// \param string The string to compare with.
	/// \return \c memcmp result.

	int compare(const StaticString &string) const throw();

	/// Return the number of occurences of any of the specified \a symbols in the string.
	/// \param symbols The list of symbols to count.
	/// \return The number of matching symbols in the string.
	/// \note <tt>count("x") == count("xxx")</tt>

	size_t count(const char *symbols) const throw();

	///
	/// \param symbol
	/// \param offset
	/// \return

	size_t find_first(char symbol, size_t offset = 0) const throw();

	///
	/// \param symbol
	/// \param offset Offset to the beginning of the ignored part of the string.
	/// \return

	size_t find_last(char symbol, size_t offset = End) const throw();

	///

	StaticString left(size_t size) const throw()
	{
		return StaticString(_text, min(size, _size));
	}

	///

	StaticString mid(size_t offset, size_t size = End) const throw()
	{
		return (offset < _size
			? StaticString(_text + offset, min(size, _size))
			: StaticString());
	}

	///

	StaticString right(size_t size) const throw()
	{
		return (size < _size
			? StaticString(&_text[_size - size], size)
			: *this);
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

	/// Convert to decimal \c double as much of the string as possible.
	/// \note The value must be in form "[+|-]d{d}[.d{d}][(e|E)[+|-]d{d}]".

	double to_double() const throw();

	/// Convert to decimal \c int32_t as much of the string as possible.

	int32_t to_int32() const throw();

	/// Convert to decimal \c int64_t as much of the string as possible.

	int64_t to_int64() const throw();

	/// Try to interpret the string as a raw undecorated \a value.

	bool to_number(int32_t *value) const throw();

	/**
	* \overload
	*/

	bool to_number(double *value) const throw();

	/// Convert to decimal \c double time as much of the string as possible.
	/// \note The time must be in form "[+|-][[HH:]MM:]SS[.{Z}]".

	double to_time() const throw();

	/// Convert to decimal \c uint32_t as much of the string as possible.

	uint32_t to_uint32() const throw();

	/// Convert to decimal \c uint64_t as much of the string as possible.

	uint64_t to_uint64() const throw();

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
		return (_size == string._size && !memcmp(_text, string._text, _size));
	}

	///

	bool operator !=(const StaticString &string) const throw()
	{
		return (_size != string._size || memcmp(_text, string._text, _size));
	}

public:

	///

	static const size_t End = SIZE_MAX;

private:

	char   *_text;
	size_t  _size;

private:

	StaticString(size_t size) throw()
		: _size(size)
	{
	}

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
