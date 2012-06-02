/// \file
/// \brief Static string.
/// \note The idea behind the static string with length is that the length of the string is almost always known.

#ifndef __Y_STATIC_STRING_H
#define __Y_STATIC_STRING_H

#include <Yttrium/allocator.h>
#include <Yttrium/types.h>
#include <Yttrium/utils.h>

// We don't need const char * operations since we can convert the vast majority of strings
// (even the literal ones) to StaticStrings of a specified length.

namespace Yttrium
{

class String;

/// Static string wrapper class.
/// \note Static strings can't implicitly hold \c nullptr and therefore should always be valid.
/// \note Static strings may have no zero terminator.

class Y_API StaticString
{
	friend class String;

public:

	/// Construct an empty StaticString.

	StaticString() noexcept
		: _text(const_cast<char *>(&Null))
		, _size(0)
	{
	}

	/// Construct a StaticString from the C-string.
	/// \param text Source text.

	StaticString(const char *text) noexcept;

	/// Construct a StaticString from the \a text of known \a size.
	/// \param text Source text.
	/// \param size Source text size.

	StaticString(const char *text, size_t size) noexcept
		: _text(const_cast<char *>(text))
		, _size(size)
	{
	}

public:

	/// Compares the string with the specified \a string.
	/// \param string The string to compare with.
	/// \return Negative result if the \a string is greater, positive if it is less, zero otherwise.

	int compare(const StaticString &string) const noexcept;

	///

	const char *const_text() const noexcept
	{
		return _text;
	}

	/// Return the number of occurences of any of the specified \a symbols in the string.
	/// \param symbols The list of symbols to count.
	/// \return The number of matching symbols in the string.
	/// \note <tt>count("x") == count("xxx")</tt>

	size_t count(const char *symbols) const noexcept;

	///

	bool ends_with(const StaticString &substring) const noexcept;

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

	bool is_empty() const noexcept
	{
		return !_size;
	}

	///

	StaticString left(size_t size) const noexcept
	{
		return StaticString(_text, min(size, _size));
	}

	///

	StaticString mid(size_t offset, size_t size = End) const noexcept
	{
		return (offset < _size
			? StaticString(&_text[offset], min(size, _size - offset))
			: StaticString());
	}

	///

	StaticString right(size_t size) const noexcept
	{
		return (size < _size
			? StaticString(&_text[_size - size], size)
			: *this);
	}

	/// Return the string size.
	/// \return String size.

	size_t size() const noexcept
	{
		return _size;
	}

	/// Return the string text pointer.
	/// \return String text pointer.

	char *text() noexcept
	{
		return _text;
	}

	/**
	* \overload
	*/

	const char *text() const noexcept
	{
		return _text;
	}

	/// Convert to decimal \c double as much of the string as possible.
	/// \note The value must be in form "[+|-]d{d}[.d{d}][(e|E)[+|-]d{d}]".

	double to_double() const noexcept;

	/// Convert to decimal \c int32_t as much of the string as possible.

	int32_t to_int32() const noexcept;

	/// Convert to decimal \c int64_t as much of the string as possible.

	int64_t to_int64() const noexcept;

	/// Try to interpret the string as a raw undecorated \a value.

	bool to_number(int32_t *value) const noexcept;

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

	const char &operator [](size_t index) const noexcept
	{
		return _text[index];
	}

	///

	bool operator <(const StaticString &string) const noexcept
	{
		return (compare(string) < 0);
	}

	///

	bool operator >(const StaticString &string) const noexcept
	{
		return (compare(string) > 0);
	}

	///

	bool operator <=(const StaticString &string) const noexcept
	{
		return (compare(string) <= 0);
	}

	///

	bool operator >=(const StaticString &string) const noexcept
	{
		return (compare(string) >= 0);
	}

	///

	bool operator ==(const StaticString &string) const noexcept;

	///

	bool operator !=(const StaticString &string) const noexcept;

public:

	///

	static const size_t End = SIZE_MAX;

private:

	char   *_text;
	size_t  _size;

private:

	StaticString(size_t size) noexcept
		: _size(size)
	{
	}

private:

	static const char Null = '\0';
};

} // namespace Yttrium

#if Y_HAVE_USER_LITERALS

	///

	Yttrium::StaticString operator "" _y(const char *text, size_t size) noexcept
	{
		return StaticString(text, size);
	}

	#define Y_S(literal) literal##_y

#else

	///

	#define Y_S(literal) Yttrium::StaticString(literal, sizeof(literal) - 1)

#endif

#endif // __Y_STATIC_STRING_H
