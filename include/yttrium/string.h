/// \file
/// \brief Text strings.

#ifndef _include_yttrium_string_h_
#define _include_yttrium_string_h_

#include <yttrium/static_string.h>

namespace Yttrium
{
	///
	class ByReference {};

	///
	/// \note Strings with allocated storage always store a null terminator.
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
		String(const StaticString& string, const ByReference&, Allocator* allocator = DefaultAllocator)
			: StaticString(string), _allocator(allocator) {}

		/// Destructor.
		~String();

	public:

		///
		Allocator* allocator() const { return _allocator; }

		///
		char* text() { return const_cast<char*>(_text); }
		using StaticString::text;

		String& operator=(const StaticString&);
		String& operator=(const String& string) { return *this = StaticString(string); }
		String& operator=(String&&) noexcept;

		char& operator[](size_t offset) { return const_cast<char*>(_text)[offset]; }
		using StaticString::operator[];

	private:
		size_t _capacity = 0;
		Allocator* _allocator = nullptr;
		struct Private;
		friend Private;
	};
}

#endif
