/// \file
/// \brief Text strings.

#ifndef _include_yttrium_string_h_
#define _include_yttrium_string_h_

#include <yttrium/static_string.h>

namespace Yttrium
{
	struct DateTime;

	///
	/// \note Strings with allocated storage always store a zero terminator.
	class Y_API String : public StaticString
	{
	public:

		///
		String(Allocator* allocator = DefaultAllocator) : _allocator(allocator) {}

		/// Copy constructor.
		/// \param string The source string.
		String(const String& string);

		///
		String(String&& string);

		///
		explicit String(const StaticString& string, Allocator* allocator = DefaultAllocator);

		///
		String(const char* text, size_t size, Allocator* allocator = DefaultAllocator);

		///
		String(const char* text, Allocator* allocator = DefaultAllocator);

		/// Preallocating constructor.
		explicit String(size_t size, Allocator* allocator = DefaultAllocator);

		/// Concatenating constructor.
		String(const StaticString& left, const StaticString& right, Allocator* allocator = DefaultAllocator);

		///
		String(const StaticString& left, char right, Allocator* allocator = DefaultAllocator);

		///
		String(char left, const StaticString& right, Allocator* allocator = DefaultAllocator);

		/// Destructor.
		~String();

	public:

		/// Reference constructor.
		String(const char* text, size_t size, const ByReference&, Allocator* allocator = DefaultAllocator)
			: StaticString(text, size), _allocator(allocator) {}

		///
		String(const StaticString& string, const ByReference&, Allocator* allocator = DefaultAllocator)
			: StaticString(string), _allocator(allocator) {}

		///
		String(const char* text, const ByReference&, Allocator* allocator = DefaultAllocator)
			: StaticString(text), _allocator(allocator) {}

	public:

		///
		Allocator* allocator() const { return _allocator; }

		/// Append the specified \a string.
		String& append(const char* text, size_t size);

		///
		String& append(const StaticString& string) { return append(string.text(), string.size()); }

		///
		String& append(const char*);

		/// Append the specified \a symbol to the string \a count times.
		String& append(char symbol, size_t count = 1);

		/// Append decimal \a value to the string.
		/// \param value Value to append.
		/// \param width Minimal number width.
		/// \param zeros Should the value be prepended with zeros instead of spaces to fit \a width?
		/// \return
		String& append_dec(int32_t value, int width = 0, bool zeros = false) { return append_dec(int64_t{value}, width, zeros); }

		///
		String& append_dec(uint32_t value, int width = 0, bool zeros = false) { return append_dec(uint64_t{value}, width, zeros); }

		///
		String& append_dec(int64_t value, int width = 0, bool zeros = false);

		///
		String& append_dec(uint64_t value, int width = 0, bool zeros = false);

		///
		String& append_dec(float value);

		///
		String& append_dec(double value);

		/// Clear the string.
		String& clear();

		/// Escape (prepend) all the specified \a symbols with an escape symbol \a with in-place.
		String& escape(const char* symbols, char with) { return swap(escaped(symbols, with)); }

		///
		String escaped(const char* symbols, char with) const { return StaticString::escaped(symbols, with, _allocator); }

		///
		/// \param text
		/// \param index
		/// \note The \a text must not refer to the current string's data.
		void insert(const StaticString& text, size_t index);

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
		String& set(const char* text, size_t size);

		///
		String& set(const StaticString& string) { return set(string.text(), string.size()); }

		///
		String& set(const char*);

		///
		String& set(char);

		///
		String& swap(String*);

		///
		String& swap(String&&);

		///
		char* text() { return const_cast<char*>(_text); }
		using StaticString::text;

		///
		char* text_at(size_t index) { return const_cast<char*>(_text) + index; }
		using StaticString::text_at;

		///
		String& trim();

		String& operator=(char s) { return set(s); }
		String& operator=(const char* s) { return set(s); }
		String& operator=(const StaticString& s) { return set(s); }
		String& operator=(const String& s) { return set(s); }

		String& operator+=(char s) { return append(s); }
		String& operator+=(const StaticString& s) { return append(s); }

		char& operator[](size_t offset) { return const_cast<char*>(_text)[offset]; }
		using StaticString::operator[];

	public:

		/// Convert a decimal value to the string.
		/// \param value Value to convert.
		/// \param width Minimal number width.
		/// \param zeros Should the value be prepended with zeros to fit \a width?
		/// \return
		static String from_dec(int32_t value, Allocator* allocator = DefaultAllocator)
		{
			return String(11, allocator).append_dec(value);
		}

		///
		static String from_dec(uint32_t value, Allocator* allocator = DefaultAllocator)
		{
			return String(10, allocator).append_dec(value);
		}

		///
		static String from_dec(int64_t value, Allocator* allocator = DefaultAllocator)
		{
			return String(21, allocator).append_dec(value);
		}

		///
		static String from_dec(uint64_t value, Allocator* allocator = DefaultAllocator)
		{
			return String(20, allocator).append_dec(value);
		}

		///
		static String from_dec(float value, Allocator* allocator = DefaultAllocator)
		{
			return String(16, allocator).append_dec(value); // TODO: Find the exact required reserve size.
		}

		///
		static String from_dec(double value, Allocator* allocator = DefaultAllocator)
		{
			return String(32, allocator).append_dec(value); // TODO: Find the exact required reserve size.
		}

		///
		static String format(const DateTime& date_time, const char* format, Allocator* allocator = DefaultAllocator);

	private:

		Y_PRIVATE void grow(size_t buffer_size);
		Y_PRIVATE void init();
		Y_PRIVATE void init(const char* string, size_t size);
		Y_PRIVATE const char* init(size_t buffer_size);

	private:

		// (_buffer_size  > 0) => dynamic string; ref_counter = *reinterpret_cast<size_t*>(_text - sizeof(size_t))
		// (_buffer_size == 0) => static string

		size_t _buffer_size = 0;
		Allocator* _allocator = nullptr;
	};

	///
	inline String& operator<<(String& lhs, char rhs)
	{
		return lhs.append(rhs);
	}

	///
	inline String& operator<<(String& lhs, const StaticString& rhs)
	{
		return lhs.append(rhs);
	}

	///
	inline String& operator<<(String& lhs, int32_t rhs)
	{
		return lhs.append_dec(rhs);
	}

	///
	inline String& operator<<(String& lhs, uint32_t rhs)
	{
		return lhs.append_dec(rhs);
	}

	///
	inline String& operator<<(String& lhs, int64_t rhs)
	{
		return lhs.append_dec(rhs);
	}

	///
	inline String& operator<<(String& lhs, uint64_t rhs)
	{
		return lhs.append_dec(rhs);
	}

	///
	inline String& operator<<(String& lhs, float rhs)
	{
		return lhs.append_dec(rhs);
	}

	///
	inline String& operator<<(String& lhs, double rhs)
	{
		return lhs.append_dec(rhs);
	}
}

#endif
