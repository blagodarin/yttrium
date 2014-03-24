/// \file
/// \brief Text strings.

#ifndef __Y_STRING_H
#define __Y_STRING_H

#include <yttrium/static_string.h>

namespace Yttrium
{

///
/// \note Strings with an allocated storage always store a zero terminator.

class Y_API String: public StaticString
{
public:

	///

	inline String(Allocator *allocator = DefaultAllocator) noexcept;

	/// Copy constructor.
	/// \param string The source string.

	String(const String &string) noexcept;

	/**
	* \overload
	* \param string
	*/

	String(String &&string) noexcept;

	///

	String(const StaticString &string, Allocator *allocator = DefaultAllocator) noexcept;

	/**
	* \overload
	* \param text
	* \param size
	* \param allocator
	*/

	String(const char *text, size_t size, Allocator *allocator = DefaultAllocator) noexcept;

	/**
	* \overload
	* \param text
	* \param allocator
	*/

	String(const char *text, Allocator *allocator = DefaultAllocator) noexcept;

	/// Preallocating constructor.

	explicit String(size_t size, Allocator *allocator = DefaultAllocator) noexcept;

	/// Concatenating constructor.
	/// \param left The left part.
	/// \param right The right part.
	/// \param allocator The allocator to use.

	String(const StaticString &left, const StaticString &right, Allocator *allocator = DefaultAllocator) noexcept;

	/**
	* \overload
	* \param left
	* \param right
	* \param allocator
	*/

	String(const StaticString &left, char right, Allocator *allocator = DefaultAllocator) noexcept;

	/**
	* \overload
	* \param left
	* \param right
	* \param allocator
	*/

	String(char left, const StaticString &right, Allocator *allocator = DefaultAllocator) noexcept;

	/// Destructor.

	~String() noexcept;

public:

	/// Reference constructor.
	/// \param text
	/// \param size
	/// \param allocator

	inline String(const char *text, size_t size, const ByReference &, Allocator *allocator = DefaultAllocator) noexcept;

	/**
	* \overload
	* \param string
	* \param allocator
	*/

	inline String(const StaticString &string, const ByReference &, Allocator *allocator = DefaultAllocator) noexcept;

	/**
	* \overload
	* \param text
	* \param allocator
	*/

	inline String(const char *text, const ByReference &, Allocator *allocator = DefaultAllocator) noexcept;

public:

	///

	inline Allocator *allocator() const noexcept;

	/// Append the specified \a string.

	String &append(const char *text, size_t size) noexcept;

	/**
	* \overload
	* \return
	*/

	inline String &append(const StaticString &) noexcept;

	/**
	* \overload
	* \return
	*/

	String &append(const char *) noexcept;

	/// Append the specified \a symbol to the string \a count times.

	String &append(char symbol, size_t count = 1) noexcept;

	/// Append decimal \a value to the string.
	/// \param value Value to append.
	/// \param width Minimal number width.
	/// \param zeros Should the value be prepended with zeros instead of spaces to fit \a width?
	/// \return

	inline String &append_dec(int32_t value, int width = 0, bool zeros = false) noexcept;

	/**
	* \overload
	* \param value
	* \param width
	* \param zeros
	* \return
	*/

	inline String &append_dec(uint32_t value, int width = 0, bool zeros = false) noexcept;

	/**
	* \overload
	* \param value
	* \param width
	* \param zeros
	* \return
	*/

	String &append_dec(int64_t value, int width = 0, bool zeros = false) noexcept;

	/**
	* \overload
	* \param value
	* \param width
	* \param zeros
	* \return
	*/

	String &append_dec(uint64_t value, int width = 0, bool zeros = false) noexcept;

	/**
	* \overload
	* \param value
	* \return
	*/

	String &append_dec(float value) noexcept;

	/**
	* \overload
	* \param value
	* \return
	*/

	String &append_dec(double value) noexcept;

	/// Clear the string.

	String &clear() noexcept;

	/// Escape (prepend) all the specified \a symbols with an escape symbol \a with in-place.

	inline String &escape(const char *symbols, char with) noexcept;

	///
	/// \param symbols
	/// \param with
	/// \return
	/// \see StaticString::escaped

	inline String escaped(const char *symbols, char with) const noexcept;

	///
	/// \param text
	/// \param index
	/// \note The \a text must not refer to the current string's data.

	void insert(const StaticString &text, size_t index) noexcept;

	/// Insert a \a symbol at the specified \a index.

	void insert(char symbol, size_t index) noexcept;

	/// Remove \a size symbols starting at the specified \a index.

	void remove(size_t index, size_t size = 1) noexcept;

	/// Reserve the space in the string for \a len symbols.

	void reserve(size_t size) noexcept;

	/// Resize the string, filling the new part with random data.

	void resize(size_t size) noexcept;

	/// Truncate the string to the specified \a size.

	void truncate(size_t size) noexcept;

	/// Set the string to the specified \a string.

	String &set(const char *text, size_t size) noexcept;

	/**
	* \overload
	* \return
	*/

	inline String &set(const StaticString &) noexcept;

	/**
	* \overload
	* \return
	*/

	String &set(const char *) noexcept;

	/**
	* \overload
	* \return
	*/

	String &set(char) noexcept;

	///

	String &swap(String *) noexcept;

	/**
	* \overload
	* \return
	*/

	String &swap(String &&) noexcept;

	///

	String &trim() noexcept;

public:

	///

	inline String &operator =(char symbol) noexcept;

	/**
	* \overload
	* \return
	*/

	inline String &operator =(const char *) noexcept;

	/**
	* \overload
	* \return
	*/

	inline String &operator =(const StaticString &) noexcept;

	/**
	* \overload
	* \return
	*/

	inline String &operator =(const String &) noexcept;

	///

	inline String &operator +=(char) noexcept;

	/**
	* \overload
	* \return
	*/

	inline String &operator +=(const StaticString &) noexcept;

public:

	/// Convert a decimal value to the string.
	/// \param value Value to convert.
	/// \param width Minimal number width.
	/// \param zeros Should the value be prepended with zeros to fit \a width?
	/// \return

	inline static String from_dec(int32_t value, int width = 0, bool zeros = false) noexcept;

	/**
	* \overload
	* \param value
	* \param width
	* \param zeros
	* \return
	*/

	inline static String from_dec(uint32_t value, int width = 0, bool zeros = false) noexcept;

	/**
	* \overload
	* \param value
	* \param width
	* \param zeros
	* \return
	*/

	inline static String from_dec(int64_t value, int width = 0, bool zeros = false) noexcept;

	/**
	* \overload
	* \param value
	* \param width
	* \param zeros
	* \return
	*/

	inline static String from_dec(uint64_t value, int width = 0, bool zeros = false) noexcept;

	/**
	* \overload
	* \param value
	* \return
	*/

	inline static String from_dec(float value) noexcept;

	/**
	* \overload
	* \param value
	* \return
	*/

	inline static String from_dec(double value) noexcept;

private:

	Y_PRIVATE void grow(size_t buffer_size);
	Y_PRIVATE void init();
	Y_PRIVATE void init(const char* string, size_t size);
	Y_PRIVATE const char* init(size_t buffer_size);

private:

	// (_buffer_size  > 0) => dynamic string; ref_counter = *reinterpret_cast<size_t*>(_text - sizeof(size_t))
	// (_buffer_size == 0) => static string

	size_t     _buffer_size;
	Allocator *_allocator;
};

/// Concatenate the arguments into a single string.
/// \param left Left operand.
/// \param right Right operand.
/// \return Concatenation result.

inline String operator +(const StaticString &left, const StaticString &right) noexcept;

/**
* \overload
* \param left Left operand.
* \param right Right operand.
* \return Concatenation result.
*/

inline String operator +(const StaticString &left, char right) noexcept;

/**
* \overload
* \param left Left operand.
* \param right Right operand.
* \return Concatenation result.
*/

inline String operator +(char left, const StaticString &right) noexcept;

/// Append the \a right value to the \a left one.

inline String &operator <<(String &left, char right) noexcept;

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, const StaticString &right) noexcept;

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, int32_t right) noexcept;

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, uint32_t right) noexcept;

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, int64_t right) noexcept;

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, uint64_t right) noexcept;

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, float right) noexcept;

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, double right) noexcept;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String::String(Allocator *allocator) noexcept
	: _buffer_size(0)
	, _allocator(allocator)
{
}

String::String(const char *text, size_t size, const ByReference &, Allocator *allocator) noexcept
	: StaticString(text, size)
	, _buffer_size(0)
	, _allocator(allocator)
{
}

String::String(const StaticString &string, const ByReference &, Allocator *allocator) noexcept
	: StaticString(string)
	, _buffer_size(0)
	, _allocator(allocator)
{
}

String::String(const char *text, const ByReference &, Allocator *allocator) noexcept
	: StaticString(text)
	, _buffer_size(0)
	, _allocator(allocator)
{
}

Allocator *String::allocator() const noexcept
{
	return const_cast<Allocator *>(_allocator);
}

String &String::append(const StaticString &string) noexcept
{
	return append(string.text(), string.size());
}

String &String::append_dec(int32_t value, int width, bool zeros) noexcept
{
	return append_dec(static_cast<int64_t>(value), width, zeros);
}

String &String::append_dec(uint32_t value, int width, bool zeros) noexcept
{
	return append_dec(static_cast<uint64_t>(value), width, zeros);
}

String &String::escape(const char *symbols, char with) noexcept
{
	return swap(escaped(symbols, with));
}

String String::escaped(const char *symbols, char with) const noexcept
{
	return StaticString::escaped(symbols, with, _allocator);
}

String &String::set(const StaticString &string) noexcept
{
	return set(string.text(), string.size());
}

String &String::operator =(char symbol) noexcept
{
	return set(symbol);
}

String &String::operator =(const char *text) noexcept
{
	return set(text);
}

String &String::operator =(const StaticString &string) noexcept
{
	return set(string);
}

String &String::operator =(const String &string) noexcept
{
	return set(string);
}

String &String::operator +=(char symbol) noexcept
{
	return append(symbol);
}

String &String::operator +=(const StaticString &string) noexcept
{
	return append(string);
}

String String::from_dec(int32_t value, int width, bool zeros) noexcept
{
	return String(11).append_dec(value, width, zeros);
}

String String::from_dec(uint32_t value, int width, bool zeros) noexcept
{
	return String(10).append_dec(value, width, zeros);
}

String String::from_dec(int64_t value, int width, bool zeros) noexcept
{
	return String(21).append_dec(value, width, zeros);
}

String String::from_dec(uint64_t value, int width, bool zeros) noexcept
{
	return String(20).append_dec(value, width, zeros);
}

String String::from_dec(float value) noexcept
{
	return String(16).append_dec(value); // TODO: Find the exact required reserve size.
}

String String::from_dec(double value) noexcept
{
	return String(32).append_dec(value); // TODO: Find the exact required reserve size.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline String operator +(const StaticString &left, const StaticString &right) noexcept
{
	return String(left, right);
}

String operator +(const StaticString &left, char right) noexcept
{
	return String(left, right);
}

String operator +(char left, const StaticString &right) noexcept
{
	return String(left, right);
}

String &operator <<(String &left, char right) noexcept
{
	return left.append(right);
}

String &operator <<(String &left, const StaticString &right) noexcept
{
	return left.append(right);
}

String &operator <<(String &left, int32_t right) noexcept
{
	return left.append_dec(right);
}

String &operator <<(String &left, uint32_t right) noexcept
{
	return left.append_dec(right);
}

String &operator <<(String &left, int64_t right) noexcept
{
	return left.append_dec(right);
}

String &operator <<(String &left, uint64_t right) noexcept
{
	return left.append_dec(right);
}

String &operator <<(String &left, float right) noexcept
{
	return left.append_dec(right);
}

String &operator <<(String &left, double right) noexcept
{
	return left.append_dec(right);
}

} // namespace Yttrium

#endif // __Y_STRING_H
