/// \file
/// \brief Text strings.

#ifndef __Y_STRING_H
#define __Y_STRING_H

#include <Yttrium/allocator.h>
#include <Yttrium/static_string.h>
#include <Yttrium/types.h>

namespace Yttrium
{

///
/// \note Strings with an allocated storage always store a zero terminator.

class Y_API String: public StaticString
{
public:

	///

	String(Allocator *allocator = DefaultAllocator) noexcept
		: _buffer_size(0)
		, _allocator(allocator)
	{
	}

	/// Copy constructor.
	/// \param string The source string.

	String(const String &string) noexcept;

	///

	String(const StaticString &string, Allocator *allocator = DefaultAllocator) noexcept;

	/**
	* \overload
	*/

	String(const char *text, size_t size, Allocator *allocator = DefaultAllocator) noexcept;

	/**
	* \overload
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
	*/

	String(const StaticString &left, char right, Allocator *allocator = DefaultAllocator) noexcept;

	/**
	* \overload
	*/

	String(char left, const StaticString &right, Allocator *allocator = DefaultAllocator) noexcept;

	/// Destructor.

	~String() noexcept
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

	struct Reference
	{
	};

	/// Reference specifier.

	static const Reference Ref;

	/// Reference constructor.
	/// \param text
	/// \param size
	/// \param allocator

	String(const char *text, size_t size, const Reference &, Allocator *allocator = DefaultAllocator) noexcept
		: StaticString(text, size)
		, _buffer_size(0)
		, _allocator(allocator)
	{
	}

	/**
	* \overload
	*/

	String(const StaticString &string, const Reference &, Allocator *allocator = DefaultAllocator) noexcept
		: StaticString(string)
		, _buffer_size(0)
		, _allocator(allocator)
	{
	}

	/**
	* \overload
	*/

	String(const char *text, const Reference &, Allocator *allocator = DefaultAllocator) noexcept
		: StaticString(text)
		, _buffer_size(0)
		, _allocator(allocator)
	{
	}

public:

	///

	Allocator *allocator() const noexcept
	{
		return const_cast<Allocator *>(_allocator);
	}

	/// Append the specified \a string.

	String &append(const char *text, size_t size) noexcept;

	/**
	* \overload
	*/

	String &append(const StaticString &string) noexcept
	{
		return append(string.text(), string.size());
	}

	/**
	* \overload
	*/

	String &append(const char *text) noexcept;

	/// Append the specified \a symbol to the string \a count times.

	String &append(char symbol, size_t count = 1) noexcept;

	/// Append decimal \a value to the string.
	/// \param value Value to append.
	/// \param width Minimal number width.
	/// \param zeros Should the value be prepended with zeros instead of spaces to fit \a width?

	String &append_dec(int32_t value, int width = 0, bool zeros = false) noexcept
	{
		return append_dec(static_cast<int64_t>(value), width, zeros);
	}

	/**
	* \overload
	*/

	String &append_dec(uint32_t value, int width = 0, bool zeros = false) noexcept
	{
		return append_dec(static_cast<uint64_t>(value), width, zeros);
	}

	/**
	* \overload
	*/

	String &append_dec(int64_t value, int width = 0, bool zeros = false) noexcept;

	/**
	* \overload
	*/

	String &append_dec(uint64_t value, int width = 0, bool zeros = false) noexcept;

	/**
	* \overload
	*/

	String &append_dec(float value) noexcept;

	/**
	* \overload
	*/

	String &append_dec(double value) noexcept;

	/// Clear the string.

	String &clear() noexcept;

	/// Escape (prepend) all the specified \a symbols with an escape symbol \a with in-place.

	String &escape(const char *symbols, char with) noexcept
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

	String escaped(const char *symbols, char with) const noexcept;

	/// Insert a \a symbol at the specified \a index.

	void insert(char symbol, size_t index) noexcept;

	/// Remove a symbol at the specified \a index.

	void remove(size_t index) noexcept;

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
	*/

	String &set(const StaticString &string) noexcept
	{
		return set(string._text, string._size);
	}

	/**
	* \overload
	*/

	String &set(const char *text) noexcept;

	/**
	* \overload
	*/

	String &set(char symbol) noexcept;

public:

	///

	String &operator =(char symbol) noexcept
	{
		return set(symbol);
	}

	/**
	* \overload
	*/

	String &operator =(const StaticString &string) noexcept
	{
		return set(string);
	}

	/**
	* \overload
	*/

	String &operator =(const String &string) noexcept
	{
		return set(string);
	}

	///

	String &operator +=(char symbol) noexcept
	{
		return append(symbol);
	}

	/**
	* \overload
	*/

	String &operator +=(const StaticString &string) noexcept
	{
		return append(string);
	}

public:

	/// Convert a decimal value to the string.
	/// \param value Value to convert.
	/// \param width Minimal number width.
	/// \param zeros Should the value be prepended with zeros to fit \a width?

	static String from_dec(int32_t value, int width = 0, bool zeros = false) noexcept
	{
		return String(11).append_dec(value, width, zeros);
	}

	/**
	* \overload
	*/

	static String from_dec(uint32_t value, int width = 0, bool zeros = false) noexcept
	{
		return String(10).append_dec(value, width, zeros);
	}

	/**
	* \overload
	*/

	static String from_dec(int64_t value, int width = 0, bool zeros = false) noexcept
	{
		return String(21).append_dec(value, width, zeros);
	}

	/**
	* \overload
	*/

	static String from_dec(uint64_t value, int width = 0, bool zeros = false) noexcept
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

inline String operator +(const StaticString &left, const StaticString &right) noexcept
{
	return String(left, right);
}

/**
* \overload
*/

inline String operator +(const StaticString &left, char right) noexcept
{
	return String(left, right);
}

/**
* \overload
*/

inline String operator +(char left, const StaticString &right) noexcept
{
	return String(left, right);
}

/// Append the \a right value to the \a left one.

inline String &operator <<(String &left, char right) noexcept
{
	return left.append(right);
}

/**
* \overload
*/

inline String &operator <<(String &left, const StaticString &right) noexcept
{
	return left.append(right);
}

/**
* \overload
*/

inline String &operator <<(String &left, int32_t right) noexcept
{
	return left.append_dec(right);
}

/**
* \overload
*/

inline String &operator <<(String &left, uint32_t right) noexcept
{
	return left.append_dec(right);
}

/**
* \overload
*/

inline String &operator <<(String &left, int64_t right) noexcept
{
	return left.append_dec(right);
}

/**
* \overload
*/

inline String &operator <<(String &left, uint64_t right) noexcept
{
	return left.append_dec(right);
}

/**
* \overload
*/

inline String &operator <<(String &left, float right) noexcept
{
	return left.append_dec(right);
}

/**
* \overload
*/

inline String &operator <<(String &left, double right) noexcept
{
	return left.append_dec(right);
}

} // namespace Yttrium

#endif // __Y_STRING_H
