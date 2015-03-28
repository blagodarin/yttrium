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

	inline String(Allocator* allocator = DefaultAllocator);

	/// Copy constructor.
	/// \param string The source string.

	String(const String& string);

	/**
	* \overload
	* \param string
	*/

	String(String&& string);

	///

	String(const StaticString& string, Allocator* allocator = DefaultAllocator);

	/**
	* \overload
	* \param text
	* \param size
	* \param allocator
	*/

	String(const char* text, size_t size, Allocator* allocator = DefaultAllocator);

	/**
	* \overload
	* \param text
	* \param allocator
	*/

	String(const char* text, Allocator* allocator = DefaultAllocator);

	/// Preallocating constructor.

	explicit String(size_t size, Allocator* allocator = DefaultAllocator);

	/// Concatenating constructor.
	/// \param left The left part.
	/// \param right The right part.
	/// \param allocator The allocator to use.

	String(const StaticString& left, const StaticString& right, Allocator* allocator = DefaultAllocator);

	/**
	* \overload
	* \param left
	* \param right
	* \param allocator
	*/

	String(const StaticString& left, char right, Allocator* allocator = DefaultAllocator);

	/**
	* \overload
	* \param left
	* \param right
	* \param allocator
	*/

	String(char left, const StaticString& right, Allocator* allocator = DefaultAllocator);

	/// Destructor.

	~String();

public:

	/// Reference constructor.
	/// \param text
	/// \param size
	/// \param allocator

	inline String(const char *text, size_t size, const ByReference &, Allocator *allocator = DefaultAllocator);

	/**
	* \overload
	* \param string
	* \param allocator
	*/

	inline String(const StaticString &string, const ByReference &, Allocator *allocator = DefaultAllocator);

	/**
	* \overload
	* \param text
	* \param allocator
	*/

	inline String(const char *text, const ByReference &, Allocator *allocator = DefaultAllocator);

public:

	///

	inline Allocator *allocator() const;

	/// Append the specified \a string.

	String &append(const char *text, size_t size);

	/**
	* \overload
	* \return
	*/

	inline String &append(const StaticString &);

	/**
	* \overload
	* \return
	*/

	String &append(const char *);

	/// Append the specified \a symbol to the string \a count times.

	String &append(char symbol, size_t count = 1);

	/// Append decimal \a value to the string.
	/// \param value Value to append.
	/// \param width Minimal number width.
	/// \param zeros Should the value be prepended with zeros instead of spaces to fit \a width?
	/// \return

	inline String &append_dec(int32_t value, int width = 0, bool zeros = false);

	/**
	* \overload
	* \param value
	* \param width
	* \param zeros
	* \return
	*/

	inline String &append_dec(uint32_t value, int width = 0, bool zeros = false);

	/**
	* \overload
	* \param value
	* \param width
	* \param zeros
	* \return
	*/

	String &append_dec(int64_t value, int width = 0, bool zeros = false);

	/**
	* \overload
	* \param value
	* \param width
	* \param zeros
	* \return
	*/

	String &append_dec(uint64_t value, int width = 0, bool zeros = false);

	/**
	* \overload
	* \param value
	* \return
	*/

	String &append_dec(float value);

	/**
	* \overload
	* \param value
	* \return
	*/

	String &append_dec(double value);

	/// Clear the string.

	String &clear();

	/// Escape (prepend) all the specified \a symbols with an escape symbol \a with in-place.

	inline String &escape(const char *symbols, char with);

	///
	/// \param symbols
	/// \param with
	/// \return
	/// \see StaticString::escaped

	inline String escaped(const char *symbols, char with) const;

	///
	/// \param text
	/// \param index
	/// \note The \a text must not refer to the current string's data.

	void insert(const StaticString &text, size_t index);

	/// Insert a \a symbol at the specified \a index.

	void insert(char symbol, size_t index);

	/// Remove \a size symbols starting at the specified \a index.

	void remove(size_t index, size_t size = 1);

	/// Reserve the space in the string for \a len symbols.

	void reserve(size_t size);

	/// Resize the string, filling the new part with random data.

	void resize(size_t size);

	/// Truncate the string to the specified \a size.

	void truncate(size_t size);

	/// Set the string to the specified \a string.

	String &set(const char *text, size_t size);

	/**
	* \overload
	* \return
	*/

	inline String &set(const StaticString &);

	/**
	* \overload
	* \return
	*/

	String &set(const char *);

	/**
	* \overload
	* \return
	*/

	String &set(char);

	///

	String &swap(String *);

	/**
	* \overload
	* \return
	*/

	String &swap(String &&);

	///

	String &trim();

public:

	///

	inline String &operator =(char symbol);

	/**
	* \overload
	* \return
	*/

	inline String &operator =(const char *);

	/**
	* \overload
	* \return
	*/

	inline String &operator =(const StaticString &);

	/**
	* \overload
	* \return
	*/

	inline String &operator =(const String &);

	///

	inline String &operator +=(char);

	/**
	* \overload
	* \return
	*/

	inline String &operator +=(const StaticString &);

public:

	/// Convert a decimal value to the string.
	/// \param value Value to convert.
	/// \param width Minimal number width.
	/// \param zeros Should the value be prepended with zeros to fit \a width?
	/// \return

	inline static String from_dec(int32_t value, int width = 0, bool zeros = false);

	/**
	* \overload
	* \param value
	* \param width
	* \param zeros
	* \return
	*/

	inline static String from_dec(uint32_t value, int width = 0, bool zeros = false);

	/**
	* \overload
	* \param value
	* \param width
	* \param zeros
	* \return
	*/

	inline static String from_dec(int64_t value, int width = 0, bool zeros = false);

	/**
	* \overload
	* \param value
	* \param width
	* \param zeros
	* \return
	*/

	inline static String from_dec(uint64_t value, int width = 0, bool zeros = false);

	/**
	* \overload
	* \param value
	* \return
	*/

	inline static String from_dec(float value);

	/**
	* \overload
	* \param value
	* \return
	*/

	inline static String from_dec(double value);

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

inline String operator +(const StaticString &left, const StaticString &right);

/**
* \overload
* \param left Left operand.
* \param right Right operand.
* \return Concatenation result.
*/

inline String operator +(const StaticString &left, char right);

/**
* \overload
* \param left Left operand.
* \param right Right operand.
* \return Concatenation result.
*/

inline String operator +(char left, const StaticString &right);

/// Append the \a right value to the \a left one.

inline String &operator <<(String &left, char right);

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, const StaticString &right);

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, int32_t right);

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, uint32_t right);

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, int64_t right);

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, uint64_t right);

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, float right);

/**
* \overload
* \param left String to append to.
* \param right Value to append.
* \result Source string with the value appended.
*/

inline String &operator <<(String &left, double right);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String::String(Allocator *allocator)
	: _buffer_size(0)
	, _allocator(allocator)
{
}

String::String(const char *text, size_t size, const ByReference &, Allocator *allocator)
	: StaticString(text, size)
	, _buffer_size(0)
	, _allocator(allocator)
{
}

String::String(const StaticString &string, const ByReference &, Allocator *allocator)
	: StaticString(string)
	, _buffer_size(0)
	, _allocator(allocator)
{
}

String::String(const char *text, const ByReference &, Allocator *allocator)
	: StaticString(text)
	, _buffer_size(0)
	, _allocator(allocator)
{
}

Allocator *String::allocator() const
{
	return const_cast<Allocator *>(_allocator);
}

String &String::append(const StaticString &string)
{
	return append(string.text(), string.size());
}

String &String::append_dec(int32_t value, int width, bool zeros)
{
	return append_dec(static_cast<int64_t>(value), width, zeros);
}

String &String::append_dec(uint32_t value, int width, bool zeros)
{
	return append_dec(static_cast<uint64_t>(value), width, zeros);
}

String &String::escape(const char *symbols, char with)
{
	return swap(escaped(symbols, with));
}

String String::escaped(const char *symbols, char with) const
{
	return StaticString::escaped(symbols, with, _allocator);
}

String &String::set(const StaticString &string)
{
	return set(string.text(), string.size());
}

String &String::operator =(char symbol)
{
	return set(symbol);
}

String &String::operator =(const char *text)
{
	return set(text);
}

String &String::operator =(const StaticString &string)
{
	return set(string);
}

String &String::operator =(const String &string)
{
	return set(string);
}

String &String::operator +=(char symbol)
{
	return append(symbol);
}

String &String::operator +=(const StaticString &string)
{
	return append(string);
}

String String::from_dec(int32_t value, int width, bool zeros)
{
	return String(11).append_dec(value, width, zeros);
}

String String::from_dec(uint32_t value, int width, bool zeros)
{
	return String(10).append_dec(value, width, zeros);
}

String String::from_dec(int64_t value, int width, bool zeros)
{
	return String(21).append_dec(value, width, zeros);
}

String String::from_dec(uint64_t value, int width, bool zeros)
{
	return String(20).append_dec(value, width, zeros);
}

String String::from_dec(float value)
{
	return String(16).append_dec(value); // TODO: Find the exact required reserve size.
}

String String::from_dec(double value)
{
	return String(32).append_dec(value); // TODO: Find the exact required reserve size.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline String operator +(const StaticString &left, const StaticString &right)
{
	return String(left, right);
}

String operator +(const StaticString &left, char right)
{
	return String(left, right);
}

String operator +(char left, const StaticString &right)
{
	return String(left, right);
}

String &operator <<(String &left, char right)
{
	return left.append(right);
}

String &operator <<(String &left, const StaticString &right)
{
	return left.append(right);
}

String &operator <<(String &left, int32_t right)
{
	return left.append_dec(right);
}

String &operator <<(String &left, uint32_t right)
{
	return left.append_dec(right);
}

String &operator <<(String &left, int64_t right)
{
	return left.append_dec(right);
}

String &operator <<(String &left, uint64_t right)
{
	return left.append_dec(right);
}

String &operator <<(String &left, float right)
{
	return left.append_dec(right);
}

String &operator <<(String &left, double right)
{
	return left.append_dec(right);
}

} // namespace Yttrium

#endif // __Y_STRING_H
