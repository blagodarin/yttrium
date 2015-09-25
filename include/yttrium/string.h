/// \file
/// \brief Text strings.

#ifndef _include_yttrium_string_h_
#define _include_yttrium_string_h_

#include <yttrium/static_string.h>

namespace Yttrium
{
	///
	/// \note Strings with allocated storage always store a zero terminator.
	class Y_API String : public StaticString
	{
	public:

		///
		String(Allocator* allocator = DefaultAllocator) : _allocator(allocator) {}

		/// Copy constructor.
		String(const String&);

		/// Move constructor.
		String(String&&) noexcept;

		///
		explicit String(const StaticString& string, Allocator* allocator = DefaultAllocator);

		///
		String(const char* text, Allocator* allocator = DefaultAllocator);

		/// Preallocating constructor.
		explicit String(size_t size, Allocator* allocator = DefaultAllocator);

		/// Destructor.
		~String();

	public:

		///
		String(const StaticString& string, const ByReference&, Allocator* allocator = DefaultAllocator)
			: StaticString(string), _allocator(allocator) {}

		///
		String(const char* text, const ByReference&, Allocator* allocator = DefaultAllocator)
			: StaticString(text), _allocator(allocator) {}

	public:

		///
		Allocator* allocator() const { return _allocator; }

		/// Clear the string.
		String& clear() noexcept;

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

		/// Reserve the space in the string for \a size symbols.
		void reserve(size_t size);

		/// Resize the string, filling the new part with random data.
		void resize(size_t size);

		/// Truncate the string to the specified \a size.
		void truncate(size_t size);

		///
		String& swap(String*) noexcept;

		///
		String& swap(String&&) noexcept;

		///
		char* text() { return const_cast<char*>(_text); }
		using StaticString::text;

		///
		String& trim();

		String& operator=(const char* string) { return *this = StaticString(string); }
		String& operator=(const StaticString&);
		String& operator=(const String& string) { return *this = StaticString(string); }
		String& operator=(String&&) noexcept;

		char& operator[](size_t offset) { return const_cast<char*>(_text)[offset]; }
		using StaticString::operator[];

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

		struct Private;
		friend Private;
	};
}

#endif
