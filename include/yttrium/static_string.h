/// \file
/// \brief Static string.

#ifndef _include_yttrium_static_string_h_
#define _include_yttrium_static_string_h_

#include <yttrium/api.h>

#include <cstdint>
#include <string>

namespace Yttrium
{
	class String;

	/// Static string.
	class Y_API StaticString
	{
		friend String;
	public:
		/// Constructs an empty string.
		StaticString() = default;

		/// Constructs a string from a C string.
		/// Constructs a valid empty string for `nullptr`.
		StaticString(const char*);

		/// Construct a string from the text of known size.
		/// The text must not be `nullptr`.
		constexpr StaticString(const char* text, size_t size) : _text(text), _size(size) {}

		///
		explicit StaticString(const std::string& string) : _text(string.data()), _size(string.size()) {}

		///
		bool ends_with(const StaticString& other) const;

		/// Escape (prepend) all the specified \a symbols with an escape symbol \a with and return the new string.
		std::string escaped(const char* symbols, char with) const;

		/// Returns `true` if the string is empty.
		constexpr bool is_empty() const { return _size == 0; }

		///
		constexpr bool is_null() const noexcept { return _text == &Null; }

		///
		constexpr StaticString left(size_t size) const { return { _text, size < _size ? size : _size }; }

		///
		constexpr StaticString right(size_t size) const
		{
			return size < _size ? StaticString(&_text[_size - size], size) : *this;
		}

		/// Returns the number of significant `char`s in the string.
		constexpr size_t size() const { return _size; }

		/// Returns the pointer to the first `char` of the string.
		constexpr const char* text() const { return _text; }

		/// Converts the string to decimal floating point value.
		/// The string must be in form `[+|-]d{d}[.d{d}][(e|E)[+|-]d{d}]`.
		float to_float() const;
		double to_double() const;

		/// Converts the string to signed decimal integer.
		/// The string must be in form `[+|-]d{d}`.
		int to_int() const;
		int32_t to_int32() const;
		int64_t to_int64() const;

		/// Tries to convert the string to decimal value.
		/// Returns `false` if the string doesn't match the numeric format.
		bool to_number(int32_t&) const;
		bool to_number(uint32_t&) const;
		bool to_number(float&) const;
		bool to_number(double&) const;

		/// Interprets the string as time, returning the corresponding number of seconds.
		/// The string must be in form `[+|-][[HH:]MM:]SS[.{Z}]`.
		double to_time() const;

		/// Converts the string to unsigned decimal integer.
		/// The string must be in form `[+]d{d}`.
		uint32_t to_uint32() const;
		uint64_t to_uint64() const;

		///
		std::string to_std() const { return std::string(_text, _size); }

		///
		StaticString trimmed() const;

		/// Returns the `char` at the specified offset.
		constexpr char operator[](size_t offset) const { return _text[offset]; }

	private:
		const char* _text = &Null;
		size_t _size = 0;

		static const char Null;
	};

	/// Returns a negative value if the first string is "greater" than
	/// the second one, a positive value if "less", zero otherwise.
	Y_API int compare(const StaticString&, const StaticString&);

	Y_API bool operator==(const StaticString&, const StaticString&);
	inline bool operator!=(const StaticString& a, const StaticString& b) { return !(a == b); }

	inline bool operator>(const StaticString& a, const StaticString& b) { return compare(a, b) > 0; }
	inline bool operator<(const StaticString& a, const StaticString& b) { return compare(a, b) < 0; }

	inline bool operator>=(const StaticString& a, const StaticString& b) { return compare(a, b) >= 0; }
	inline bool operator<=(const StaticString& a, const StaticString& b) { return compare(a, b) <= 0; }

	inline namespace Literals
	{
		///
		constexpr StaticString operator"" _s(const char* data, size_t size) { return { data, size }; }
	}

	namespace detail
	{
		inline void append_to(std::string& string, const StaticString& value) { string.append(value.text(), value.size()); }
	}
}

#endif
