/// \file
/// \brief Text strings.

#ifndef __Y_STRING_HPP
#define __Y_STRING_HPP

#include <cstring> // strcmp, strlen

#include <Yttrium/allocators.hpp>
#include <Yttrium/global.hpp>
#include <Yttrium/safe_bool.hpp>
#include <Yttrium/static_string.hpp>
#include <Yttrium/types.hpp>

namespace Yttrium
{

///

class Y_API String
{
public:

	///

	String(Allocator *allocator = HeapAllocator::instance()) throw()
		: _text(const_cast<char*>(&null))
		, _size(0)
		, _buffer_size(0)
		, _allocator(allocator)
	{
	}

	///

	String(const char *string, Allocator *allocator = HeapAllocator::instance());

	///

	String(const char *string, size_t size, Allocator *allocator = HeapAllocator::instance());

	///

	String(const StaticString &string, Allocator *allocator = HeapAllocator::instance());

	/// Copy constructor.
	/// \param string The source string.

	String(const String &string);

	/// Extended copy constructor which allows to specify an allocator.
	/// \param string The source string.
	/// \param allocator The allocator to use.

	String(const String &string, Allocator *allocator);

	/// Preallocating constructor.

	explicit String(size_t size, Allocator *allocator = HeapAllocator::instance());

	/// Concatenating constructor.
	/// \param left The left part.
	/// \param right The right part.
	/// \param allocator The allocator to use.

	String(char left, const StaticString& right, Allocator *allocator = HeapAllocator::instance());

	/**
	* \overload
	*/

	String(const char *left, const StaticString &right, Allocator *allocator = HeapAllocator::instance());

	/**
	* \overload
	*/

	String(const StaticString &left, char right, Allocator *allocator = HeapAllocator::instance());

	/**
	* \overload
	*/

	String(const StaticString &left, const char *right, Allocator *allocator = HeapAllocator::instance());

	/**
	* \overload
	*/

	String(const StaticString &left, const StaticString &right, Allocator *allocator = HeapAllocator::instance());

	/// Destructor.

	~String() throw()
	{
		if (_buffer_size)
		{
			size_t *references = reinterpret_cast<size_t *>(_text) - 1;
			if (!--*references)
			{
				_allocator->deallocate(references);
			}
		}
	}

public:

	///

	Allocator *allocator() const throw()
	{
		return const_cast<Allocator *>(_allocator);
	}

	/// Append the specified string \a s.

	String &append(const char *string)
	{
		return append(string, strlen(string));
	}

	/// Append the specified \a string of known \a size.

	String &append(const char *string, size_t size);

	/// Append the specified \a string.

	String &append(const StaticString &string)
	{
		return append(string.text(), string.size());
	}

	/**
	* \overload
	*/

	String &append(const String &string)
	{
		return append(string._text, string._size);
	}

	/// Append the specified \a symbol to the string \a count times.

	String &append(char symbol, size_t count = 1);

	/// Append decimal \a value to the string.
	/// \param value Value to append.
	/// \param width Minimal number width.
	/// \param zeros Should the value be prepended with zeros instead of spaces to fit \a width?

	String &append_dec(int32_t value, int width = 0, bool zeros = false)
	{
		return append_dec(static_cast<int64_t>(value), width, zeros);
	}

	/**
	* \overload
	*/

	String &append_dec(uint32_t value, int width = 0, bool zeros = false)
	{
		return append_dec(static_cast<uint64_t>(value), width, zeros);
	}

	/**
	* \overload
	*/

	String &append_dec(int64_t value, int width = 0, bool zeros = false);

	/**
	* \overload
	*/

	String &append_dec(uint64_t value, int width = 0, bool zeros = false);

	/**
	* \overload
	*/

	String &append_dec(float value);

	/**
	* \overload
	*/

	String &append_dec(double value);

	/// Clear the string.

	String &clear() throw();

	/// Return the number of occurences of any of the specified \a symbols in the string.
	/// \param symbols The list of symbols to count.
	/// \return The number of matching symbols in the string.
	/// \note If the same symbol appears several times in the \a symbols list,
	/// it will count the same number of times.

	size_t count(const char *symbols) const throw();

	/// Escape (prepend) all the specified \a symbols with an escape symbol \a with in-place.

	String &escape(const char *symbols, char with)
	{
		String result = escaped(symbols, with);
		if (result._size != _size)
		{
			// NOTE: We can swap the pointers (and allocators) here.

			set(result._text, result._size);
		}
		return *this;
	}

	/// Escape (prepend) all the specified \a symbols with an escape symbol \a with and return the new string.

	String escaped(const char *symbols, char with) const;

	/// Insert a \a symbol at the specified \a index.

	void insert(char symbol, size_t index);

	/// Remove a symbol at the specified \a index.

	void remove(size_t index);

	/// Reserve the space in the string for \a len symbols.

	void reserve(size_t size);

	/// Convert to decimal \c double as much of the string as possible.
	/// \note The value must be in form "[+|-]d{d}[.d{d}][(e|E)[+|-]d{d}]".

	double to_double() const throw();

	/// Convert to decimal \c int32_t as much of the string as possible.

	int32_t to_int32() const throw();

	/// Convert to decimal \c int64_t as much of the string as possible.

	int64_t to_int64() const throw();

	/// Try to interpret the string as a raw undecorated \a value.

	bool to_number(int32_t &value) const throw();

	/**
	* \overload
	*/

	bool to_number(double &value) const throw();

	/// Convert to decimal \c double time as much of the string as possible.
	/// \note The time must be in form "[+|-][[HH:]MM:]SS[.{Z}]".

	double to_time() const throw();

	/// Convert to decimal \c uint32_t as much of the string as possible.

	uint32_t to_uint32() const throw();

	/// Convert to decimal \c uint64_t as much of the string as possible.

	uint64_t to_uint64() const throw();

	/// Truncate the string to the specified \a size.

	void truncate(size_t size);

	/// Set the string to the specified \a string.

	String &set(const String &string);

	/// Set the string to the specified \a string.

	String &set(const char *string)
	{
		return set(string, strlen(string));
	}

	/// Set the string to the specified \a string of known \a size.

	String &set(const char *string, size_t size);

	/// Set the string to the specified \a symbol.

	String &set(char symbol);

	///

	size_t size() const
	{
		return _size;
	}

	/// Return the pointer to the string text.

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

	operator StaticString() const throw()
	{
		return StaticString(_text, _size);
	}

	///
	/// \note This operator may be dangerous if misused (on i.e. reference strings).

	char &operator [](size_t index) throw()
	{
		return _text[index];
	}

	///

	const char &operator [](size_t index) const throw()
	{
		return _text[index];
	}

	///

	String operator +(char symbol) const
	{
		return String(StaticString(*this), symbol);
	}

	/**
	* \overload
	*/

	String operator +(const char *string) const
	{
		return String(StaticString(*this), string);
	}

	/**
	* \overload
	*/

	String operator +(const StaticString &string) const
	{
		return String(StaticString(*this), string);
	}

	/**
	* \overload
	*/

	String operator +(const String &string) const
	{
		return String(StaticString(*this), StaticString(string));
	}

	///

	String &operator =(char symbol)
	{
		return set(symbol);
	}

	/**
	* \overload
	*/

	String &operator =(const char *string)
	{
		return set(string, strlen(string));
	}

	/**
	* \overload
	*/

	String &operator =(const StaticString &string)
	{
		return set(string.text(), string.size());
	}

	/**
	* \overload
	*/

	String &operator =(const String &string)
	{
		return set(string._text, string._size);
	}

	///

	String &operator +=(char symbol)
	{
		return append(symbol);
	}

	/**
	* \overload
	*/

	String &operator +=(const char *string)
	{
		return append(string, strlen(string));
	}

	/**
	* \overload
	*/

	String &operator +=(const StaticString &string)
	{
		return append(string.text(), string.size());
	}

	/**
	* \overload
	*/

	String &operator +=(const String &string)
	{
		return append(string._text, string._size);
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
		return (strcmp(_text, string.text()) < 0);
	}

	/**
	* \overload
	*/

	bool operator <(const String &string) const throw()
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
		return (strcmp(_text, string.text()) > 0);
	}

	/**
	* \overload
	*/

	bool operator >(const String &string) const throw()
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
		return (strcmp(_text, string.text()) <= 0);
	}

	/**
	* \overload
	*/

	bool operator <=(const String &string) const throw()
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
		return (strcmp(_text, string.text()) >= 0);
	}

	/**
	* \overload
	*/

	bool operator >=(const String &string) const throw()
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
		return (_size == right.size() && strcmp(_text, right.text()) == 0);
	}

	/**
	* \overload
	*/

	bool operator ==(const String &right) const throw()
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
		return (_size != string.size() || strcmp(_text, string.text()) != 0);
	}

	/**
	* \overload
	*/

	bool operator !=(const String &string) const throw()
	{
		return (_size != string._size || strcmp(_text, string._text) != 0);
	}

public:

	/// Convert a decimal value to the string.
	/// \param value Value to convert.
	/// \param width Minimal number width.
	/// \param zeros Should the value be prepended with zeros to fit \a width?

	static String from_dec(int32_t value, int width = 0, bool zeros = false)
	{
		return String(11).append_dec(value, width, zeros);
	}

	/**
	* \overload
	*/

	static String from_dec(uint32_t value, int width = 0, bool zeros = false)
	{
		return String(10).append_dec(value, width, zeros);
	}

	/**
	* \overload
	*/

	static String from_dec(int64_t value, int width = 0, bool zeros = false)
	{
		return String(21).append_dec(value, width, zeros);
	}

	/**
	* \overload
	*/

	static String from_dec(uint64_t value, int width = 0, bool zeros = false)
	{
		return String(20).append_dec(value, width, zeros);
	}

	/// Create a reference string.

	static String reference(const char *string) throw()
	{
		return String(string, strlen(string), Y_SAFE_BOOL(true));
	}

	/**
	* \overload
	*/

	static String reference(const char* string, size_t size) throw()
	{
		return String(string, size, Y_SAFE_BOOL(true));
	}

	/**
	* \overload
	*/

	static String reference(const StaticString& string) throw()
	{
		return String(string, Y_SAFE_BOOL(true));
	}

	/**
	* \overload
	*/

	static String reference(const String &string) throw()
	{
		return String(string, Y_SAFE_BOOL(true));
	}

private:

	String(const char *string, size_t size, SafeBool) throw()
		: _text(const_cast<char *>(string))
		, _size(size)
		, _buffer_size(0)
	{
	}

	String(const StaticString &string, SafeBool) throw()
		: _text(const_cast<char *>(string.text()))
		, _size(string.size())
		, _buffer_size(0)
	{
	}

	String(const String &string, SafeBool) throw()
		: _text(string._text)
		, _size(string._size)
		, _buffer_size(0)
	{
	}

private:

	void grow(size_t buffer_size);

	void init();

	void init(const char *string, size_t size);

	char *init(size_t buffer_size);

private:

	// (_buffer_size  > 0) => real string; ref_counter = *reinterpret_cast<size_t*>(_text - sizeof(size_t))
	// (_buffer_size == 0) => 'const char *' string

	char      *_text;
	size_t    _size;
	size_t    _buffer_size;
	Allocator *_allocator;

private:

	static const char null = '\0';
};

/// Concatenate the arguments into a single string.

inline String operator +(const char left, const StaticString &right)
{
	return String(left, right);
}

/**
* \overload
*/

inline String operator +(char left, const String &right)
{
	return String(left, right);
}

/**
* \overload
*/

inline String operator +(const StaticString &left, char right)
{
	return String(left, right);
}

/**
* \overload
*/

inline String operator +(const StaticString &left, const StaticString &right)
{
	return String(left, right);
}

/**
* \overload
*/

inline String operator +(const StaticString &left, const String &right)
{
	return String(left, StaticString(right));
}

/// Append the \a right value to the \a left one.

inline String &operator <<(String &left, char right)
{
	return left.append(right);
}

/**
* \overload
*/

inline String &operator <<(String &left, const char *right)
{
	return left.append(right, strlen(right));
}

/**
* \overload
*/

inline String &operator <<(String &left, const StaticString &right)
{
	return left.append(right.text(), right.size());
}

/**
* \overload
*/

inline String &operator <<(String &left, const String &right)
{
	return left.append(right.text(), right.size());
}

/**
* \overload
*/

inline String &operator <<(String &left, int32_t right)
{
	return left.append_dec(right);
}

/**
* \overload
*/

inline String &operator <<(String &left, uint32_t right)
{
	return left.append_dec(right);
}

/**
* \overload
*/

inline String &operator <<(String &left, int64_t right)
{
	return left.append_dec(right);
}

/**
* \overload
*/

inline String &operator <<(String &left, uint64_t right)
{
	return left.append_dec(right);
}

/**
* \overload
*/

inline String &operator <<(String &left, float right)
{
	return left.append_dec(right);
}

/**
* \overload
*/

inline String& operator <<(String &left, double right)
{
	return left.append_dec(right);
}

} // namespace Yttrium

#endif // __Y_STRING_HPP
