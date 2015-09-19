/// \file
/// \brief Static string.
/// \note The idea behind the static string with length is that the length of the string is almost always known.

#ifndef _include_yttrium_static_string_h_
#define _include_yttrium_static_string_h_

#include <yttrium/alloca.h>
#include <yttrium/types.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iosfwd>

namespace Yttrium
{
	class String;

	extern Y_API const char StringNull;

	/// Static string wrapper class.
	/// \note Static strings can't implicitly hold \c nullptr and therefore should always be valid.
	/// \note Static strings may have no zero terminator.
	class Y_API StaticString
	{
		friend String;
	public:

		///
		static constexpr size_t End = SIZE_MAX;

		/// Construct an empty StaticString.
		StaticString() = default;

		/// Construct a StaticString from the C-string.
		/// \param text Source text.
		StaticString(const char* text);

		/// Construct a StaticString from the \a text of known \a size.
		/// \param text Source text.
		/// \param size Source text size.
		StaticString(const char* text, size_t size) : _text(const_cast<char*>(text)), _size(size) {}

		/// Compares the string with the specified \a string.
		/// \param string The string to compare with.
		/// \return Negative result if the \a string is greater, positive if it is less, zero otherwise.
		int compare(const StaticString& string) const;

		///
		const char* const_text() const { return _text; }

		/// Return the number of occurences of any of the specified \a symbols in the string.
		/// \param symbols The list of symbols to count.
		/// \return The number of matching symbols in the string.
		/// \note <tt>count("x") == count("xxx")</tt>
		size_t count(const char* symbols) const;

		///
		bool ends_with(const StaticString& substring) const;

		/// Escape (prepend) all the specified \a symbols with an escape symbol \a with and return the new string.
		String escaped(const char* symbols, char with, Allocator* allocator = DefaultAllocator) const;

		///
		StaticString file_extension() const;

		///
		size_t find_first(char symbol, size_t offset = 0) const;

		///
		/// \param symbol
		/// \param offset Offset to the beginning of the ignored part of the string.
		/// \return
		size_t find_last(char symbol, size_t offset = End) const;

		/// Check whether the string is empty.
		bool is_empty() const { return _size == 0; }

		///
		StaticString left(size_t size) const { return StaticString(_text, (size < _size ? size : _size)); }

		///
		StaticString mid(size_t offset, size_t size = End) const
		{
			return offset < _size ? StaticString(&_text[offset], _size - offset).left(size) : StaticString();
		}

		///
		StaticString right(size_t size) const { return size < _size ? StaticString(&_text[_size - size], size) : *this; }

		/// Return the string size.
		/// \return %String size.
		size_t size() const { return _size; }

		///
		const char* text() const { return _text; }

		///
		const char* text_at(size_t index) const { return _text + index; }

		///
		StaticString trimmed() const;

		/// Convert to decimal \c double as much of the string as possible.
		/// \return
		/// \note The value must be in form "[+|-]d{d}[.d{d}][(e|E)[+|-]d{d}]".
		double to_double() const;

		/// Convert to decimal \c float as much of the string as possible.
		/// \return
		/// \note The value must be in form "[+|-]d{d}[.d{d}][(e|E)[+|-]d{d}]".
		float to_float() const;

		/// Convert to decimal \c int32_t as much of the string as possible.
		int32_t to_int32() const;

		/// Convert to decimal \c int64_t as much of the string as possible.
		int64_t to_int64() const;

		/// Try to interpret the string as a raw undecorated \a value.
		bool to_number(int32_t* value) const;

		/**
		* \overload
		* \param value
		* \return
		*/
		bool to_number(float* value) const;

		/**
		* \overload
		* \param value
		* \return
		*/
		bool to_number(double* value) const;

		/// Convert to decimal \c double time as much of the string as possible.
		/// \return
		/// \note The time must be in form "[+|-][[HH:]MM:]SS[.{Z}]".
		double to_time() const;

		/// Convert to decimal \c uint32_t as much of the string as possible.
		uint32_t to_uint32() const;

		/// Convert to decimal \c uint64_t as much of the string as possible.
		uint64_t to_uint64() const;

		///
		char operator[](size_t offset) const { return _text[offset]; }

	private:

		const char* _text = &StringNull;
		size_t _size = 0;

		StaticString(size_t size) : _size(size) {}
	};

	Y_API bool operator==(const StaticString&, const StaticString&);
	Y_API bool operator!=(const StaticString&, const StaticString&);

	inline bool operator<(const StaticString& lhs, const StaticString& rhs)
	{
		return lhs.compare(rhs) < 0;
	}

	inline bool operator>(const StaticString& lhs, const StaticString& rhs)
	{
		return lhs.compare(rhs) > 0;
	}

	inline bool operator<=(const StaticString& lhs, const StaticString& rhs)
	{
		return lhs.compare(rhs) <= 0;
	}

	inline bool operator>=(const StaticString& lhs, const StaticString& rhs)
	{
		return lhs.compare(rhs) >= 0;
	}

	Y_API std::ostream& operator<<(std::ostream&, const StaticString&);

	/// Helper class for building StaticStrings from string literals.
	class S : public StaticString
	{
	public:

		/// Constructor.
		/// \param text %String literal (or any other \c char array).
		template <size_t N>
		S(const char (&text)[N])
			: StaticString(text, N - 1)
		{
		}
	};
}

///
/// \note Might crash if the string is allocated in the very end of readable space and is not zero terminated.
#define Y_ZERO_TERMINATED(result, string) \
	const char* const result = string.text(); \
	if (result[string.size()] != '\0') \
	{ \
		const_cast<const char*&>(result) = static_cast<char*>(Y_ALLOCA(string.size() + 1));\
		::memcpy(const_cast<char*>(result), string.text(), string.size()); \
		const_cast<char*>(result)[string.size()] = '\0'; \
	} \

#endif
