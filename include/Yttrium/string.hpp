/// \file
/// \brief Text strings.

#ifndef __Y_STRING_HPP
#define __Y_STRING_HPP

#include <cstring> // strlen

#include <Yttrium/allocator.hpp>
#include <Yttrium/global.hpp>
#include <Yttrium/safe_bool.hpp>
#include <Yttrium/static_string.hpp>
#include <Yttrium/types.hpp>

namespace Yttrium
{

///
/// \note Strings with an allocated storage always store a zero terminator.

class Y_API String: public StaticString
{
public:

	///

	String(Allocator *allocator = HeapAllocator::instance()) throw()
		: _buffer_size(0)
		, _allocator(allocator)
	{
	}

	/// Copy constructor.
	/// \param string The source string.

	String(const String &string);

	///

	String(const StaticString &string, Allocator *allocator = HeapAllocator::instance());

	/**
	* \overload
	*/

	String(const char *text, size_t size, Allocator *allocator = HeapAllocator::instance());

	/**
	* \overload
	*/

	String(const char *text, Allocator *allocator = HeapAllocator::instance());

	/// Preallocating constructor.

	explicit String(size_t size, Allocator *allocator = HeapAllocator::instance());

	/// Concatenating constructor.
	/// \param left The left part.
	/// \param right The right part.
	/// \param allocator The allocator to use.

	String(const StaticString &left, const StaticString &right, Allocator *allocator = HeapAllocator::instance());

	/**
	* \overload
	*/

	String(const StaticString &left, char right, Allocator *allocator = HeapAllocator::instance());

	/**
	* \overload
	*/

	String(char left, const StaticString &right, Allocator *allocator = HeapAllocator::instance());

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

	/// Reference specifier type.

	struct ReferenceSpecifier
	{
	};

	/// Reference specifier.

	static const ReferenceSpecifier Reference;

	/// Reference constructor.
	/// \param text
	/// \param size
	/// \param allocator

	String(const char *text, size_t size, const ReferenceSpecifier &, Allocator *allocator = HeapAllocator::instance()) throw()
		: StaticString(text, size)
		, _buffer_size(0)
		, _allocator(allocator)
	{
	}

	/**
	* \overload
	*/

	String(const StaticString &string, const ReferenceSpecifier &, Allocator *allocator = HeapAllocator::instance()) throw()
		: StaticString(string)
		, _buffer_size(0)
		, _allocator(allocator)
	{
	}

	/**
	* \overload
	*/

	String(const char *text, const ReferenceSpecifier &, Allocator *allocator = HeapAllocator::instance()) throw()
		: StaticString(text)
		, _buffer_size(0)
		, _allocator(allocator)
	{
	}

public:

	///

	Allocator *allocator() const throw()
	{
		return const_cast<Allocator *>(_allocator);
	}

	/// Append the specified \a string.

	String &append(const char *text, size_t size);

	/**
	* \overload
	*/

	String &append(const StaticString &string)
	{
		return append(string.text(), string.size());
	}

	/**
	* \overload
	*/

	String &append(const char *text)
	{
		return append(text, strlen(text));
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

	/// Escape (prepend) all the specified \a symbols with an escape symbol \a with in-place.

	String &escape(const char *symbols, char with)
	{
		String result = escaped(symbols, with);
		if (result._size != _size)
		{
			// NOTE: We can swap the pointers (and allocators) here.

			set(result);
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

	/// Truncate the string to the specified \a size.

	void truncate(size_t size);

	/// Set the string to the specified \a string.

	String &set(const char *text, size_t size);

	/**
	* \overload
	*/

	String &set(const StaticString &string)
	{
		return set(string._text, string._size);
	}

	/**
	* \overload
	*/

	String &set(const char *text)
	{
		return set(text, strlen(text));
	}

	/**
	* \overload
	*/

	String &set(char symbol);

public:

	///
	/// \note This operator may be dangerous if misused (on i.e. reference strings).

	char &operator [](size_t index) throw()
	{
		return _text[index];
	}

	///

	String &operator =(char symbol)
	{
		return set(symbol);
	}

	/**
	* \overload
	*/

	String &operator =(const StaticString &string)
	{
		return set(string);
	}

	///

	String &operator +=(char symbol)
	{
		return append(symbol);
	}

	/**
	* \overload
	*/

	String &operator +=(const StaticString &string)
	{
		return append(string);
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

private:

	void grow(size_t buffer_size);

	void init();

	void init(const char *string, size_t size);

	char *init(size_t buffer_size);

private:

	// (_buffer_size  > 0) => dynamic string; ref_counter = *reinterpret_cast<size_t*>(_text - sizeof(size_t))
	// (_buffer_size == 0) => static string

	size_t     _buffer_size;
	Allocator *_allocator;
};

/// Concatenate the arguments into a single string.

inline String operator +(const StaticString &left, const StaticString &right)
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

inline String operator +(char left, const StaticString &right)
{
	return String(left, right);
}

/// Append the \a right value to the \a left one.

inline String &operator <<(String &left, char right)
{
	return left.append(right);
}

/**
* \overload
*/

inline String &operator <<(String &left, const StaticString &right)
{
	return left.append(right);
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

inline String &operator <<(String &left, double right)
{
	return left.append_dec(right);
}

} // namespace Yttrium

#endif // __Y_STRING_HPP
