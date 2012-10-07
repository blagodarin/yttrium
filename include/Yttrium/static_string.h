/// \file
/// \brief Static string.
/// \note The idea behind the static string with length is that the length of the string is almost always known.

#ifndef __Y_STATIC_STRING_H
#define __Y_STATIC_STRING_H

#include <Yttrium/types.h>

namespace Yttrium
{

class String;

/// Static string wrapper class.
/// \note Static strings can't implicitly hold \c nullptr and therefore should always be valid.
/// \note Static strings may have no zero terminator.

class Y_API StaticString
{
public:

	/// Construct an empty StaticString.

	inline StaticString() noexcept;

	/// Construct a StaticString from the C-string.
	/// \param text Source text.

	StaticString(const char *text) noexcept;

	/// Construct a StaticString from the \a text of known \a size.
	/// \param text Source text.
	/// \param size Source text size.

	inline StaticString(const char *text, size_t size) noexcept;

public:

	///

	inline char &at(size_t index) noexcept;

	/**
	* \overload
	*/

	inline char at(size_t index) const noexcept;

	/// Compares the string with the specified \a string.
	/// \param string The string to compare with.
	/// \return Negative result if the \a string is greater, positive if it is less, zero otherwise.

	int compare(const StaticString &string) const noexcept;

	///

	inline char const_at(size_t index) const noexcept;

	///

	inline const char *const_text() const noexcept;

	///

	inline const char *const_text_at(size_t index) const noexcept;

	/// Return the number of occurences of any of the specified \a symbols in the string.
	/// \param symbols The list of symbols to count.
	/// \return The number of matching symbols in the string.
	/// \note <tt>count("x") == count("xxx")</tt>

	size_t count(const char *symbols) const noexcept;

	///

	bool ends_with(const StaticString &substring) const noexcept;

	/// Escape (prepend) all the specified \a symbols with an escape symbol \a with and return the new string.

	String escaped(const char *symbols, char with, Allocator* allocator = DefaultAllocator) const noexcept;

	///

	StaticString file_extension() const noexcept;

	///
	/// \param symbol
	/// \param offset
	/// \return

	size_t find_first(char symbol, size_t offset = 0) const noexcept;

	///
	/// \param symbol
	/// \param offset Offset to the beginning of the ignored part of the string.
	/// \return

	size_t find_last(char symbol, size_t offset = End) const noexcept;

	/// Check whether the string is empty.

	inline bool is_empty() const noexcept;

	///

	inline StaticString left(size_t size) const noexcept;

	///

	inline StaticString mid(size_t offset, size_t size = End) const noexcept;

	///

	inline StaticString right(size_t size) const noexcept;

	/// Return the string size.
	/// \return %String size.

	inline size_t size() const noexcept;

	/// Return the string text pointer.
	/// \return %String text pointer.

	inline char *text() noexcept;

	/**
	* \overload
	*/

	inline const char *text() const noexcept;

	///

	inline char *text_at(size_t index) noexcept;

	/**
	* \overload
	*/

	inline const char *text_at(size_t index) const noexcept;

	///

	StaticString trimmed() const noexcept;

	/// Convert to decimal \c double as much of the string as possible.
	/// \note The value must be in form "[+|-]d{d}[.d{d}][(e|E)[+|-]d{d}]".

	double to_double() const noexcept;

	/// Convert to decimal \c float as much of the string as possible.
	/// \note The value must be in form "[+|-]d{d}[.d{d}][(e|E)[+|-]d{d}]".

	float to_float() const noexcept;

	/// Convert to decimal \c int32_t as much of the string as possible.

	int32_t to_int32() const noexcept;

	/// Convert to decimal \c int64_t as much of the string as possible.

	int64_t to_int64() const noexcept;

	/// Try to interpret the string as a raw undecorated \a value.

	bool to_number(int32_t *value) const noexcept;

	/**
	* \overload
	*/

	bool to_number(float *value) const noexcept;

	/**
	* \overload
	*/

	bool to_number(double *value) const noexcept;

	/// Convert to decimal \c double time as much of the string as possible.
	/// \note The time must be in form "[+|-][[HH:]MM:]SS[.{Z}]".

	double to_time() const noexcept;

	/// Convert to decimal \c uint32_t as much of the string as possible.

	uint32_t to_uint32() const noexcept;

	/// Convert to decimal \c uint64_t as much of the string as possible.

	uint64_t to_uint64() const noexcept;

	///
	/// \note Might possibly crash if the string is allocated in the very end of a readable space.

	String zero_terminated(Allocator *allocator = DefaultAllocator) const noexcept;

public:

	///

	inline char operator [](size_t index) const noexcept;

	/**
	* \overload
	*/

	inline char &operator [](size_t index) noexcept;

	///

	inline bool operator <(const StaticString &string) const noexcept;

	///

	inline bool operator >(const StaticString &string) const noexcept;

	///

	inline bool operator <=(const StaticString &string) const noexcept;

	///

	inline bool operator >=(const StaticString &string) const noexcept;

	///

	bool operator ==(const StaticString &string) const noexcept;

	///

	bool operator !=(const StaticString &string) const noexcept;

public:

	///

	static const size_t End = SIZE_MAX;

protected:

	char   *_text;
	size_t  _size;

protected:

	inline StaticString(size_t size) noexcept;

protected:

	static const char Null = '\0';
};

/// Helper class for building StaticStrings from string literals.

class S: public StaticString
{
public:

	/// Constructor.
	/// \param text String literal (or any other \c char array).

	template <size_t N>
	S(const char (&text)[N]) noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StaticString::StaticString() noexcept
	: _text(const_cast<char *>(&Null))
	, _size(0)
{
}

StaticString::StaticString(const char *text, size_t size) noexcept
	: _text(const_cast<char *>(text))
	, _size(size)
{
}

char &StaticString::at(size_t index) noexcept
{
	return _text[index];
}

char StaticString::at(size_t index) const noexcept
{
	return _text[index];
}

char StaticString::const_at(size_t index) const noexcept
{
	return _text[index];
}

const char *StaticString::const_text() const noexcept
{
	return _text;
}

const char *StaticString::const_text_at(size_t index) const noexcept
{
	return _text + index;
}

bool StaticString::is_empty() const noexcept
{
	return !_size;
}

StaticString StaticString::left(size_t size) const noexcept
{
	return StaticString(_text, (size < _size ? size : _size));
}

StaticString StaticString::mid(size_t offset, size_t size) const noexcept
{
	return offset < _size
		? StaticString(&_text[offset], _size - offset).left(size)
		: StaticString();
}

StaticString StaticString::right(size_t size) const noexcept
{
	return size < _size
		? StaticString(&_text[_size - size], size)
		: *this;
}

size_t StaticString::size() const noexcept
{
	return _size;
}

char *StaticString::text() noexcept
{
	return _text;
}

const char *StaticString::text() const noexcept
{
	return _text;
}

char *StaticString::text_at(size_t index) noexcept
{
	return _text + index;
}

const char *StaticString::text_at(size_t index) const noexcept
{
	return _text + index;
}

char StaticString::operator [](size_t index) const noexcept
{
	return _text[index];
}

char &StaticString::operator [](size_t index) noexcept
{
	return _text[index];
}

bool StaticString::operator <(const StaticString &string) const noexcept
{
	return compare(string) < 0;
}

bool StaticString::operator >(const StaticString &string) const noexcept
{
	return compare(string) > 0;
}

bool StaticString::operator <=(const StaticString &string) const noexcept
{
	return compare(string) <= 0;
}

bool StaticString::operator >=(const StaticString &string) const noexcept
{
	return compare(string) >= 0;
}

StaticString::StaticString(size_t size) noexcept
	: _size(size)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <size_t N>
S::S(const char (&text)[N]) noexcept
	: StaticString(text, N - 1)
{
}

} // namespace Yttrium

#endif // __Y_STATIC_STRING_H
