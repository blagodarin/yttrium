#ifndef _include_yttrium_string_h_
#define _include_yttrium_string_h_

#include <yttrium/api.h>
#include <yttrium/std/string_view.h>

namespace Yttrium
{
	///
	class ByReference {};

	///
	/// \note Strings with allocated storage always store a null terminator.
	class Y_API String
	{
	public:
		String() noexcept = default;
		String(const String&);
		String(String&&) noexcept;
		explicit String(std::string_view);
		String(std::string_view string, const ByReference&) noexcept : _data{const_cast<char*>(string.data())}, _size{string.size()} {}
		~String();

		operator std::string_view() const noexcept { return {_data, _size}; }

		String& operator=(std::string_view);
		String& operator=(const String& string) { return *this = std::string_view{string.data(), string.size()}; }
		String& operator=(String&&) noexcept;

		char& operator[](size_t offset) { return _data[offset]; }
		char operator[](size_t offset) const { return _data[offset]; }

		char* data() noexcept { return _data; }
		const char* data() const noexcept { return _data; }

		bool empty() const noexcept { return !_size; }

		std::size_t size() const noexcept { return _size; }

	private:
		static char Null;

		char* _data = &Null;
		size_t _size = 0;
		size_t _capacity = 0;

		struct Private;
		friend Private;
	};

	inline bool operator==(const String& a, const String& b) noexcept { return std::string_view{a} == std::string_view{b}; }
	inline bool operator==(const String& a, std::string_view b) noexcept { return std::string_view{a} == b; }
	inline bool operator==(std::string_view a, const String& b) noexcept { return a == std::string_view{b}; }

	inline bool operator!=(const String& a, const String& b) noexcept { return !(a == b); }
	inline bool operator!=(const String& a, std::string_view b) noexcept { return !(a == b); }
	inline bool operator!=(std::string_view a, const String& b) noexcept { return !(a == b); }

	inline bool operator>(const String& a, const String& b) noexcept { return std::string_view{a} > std::string_view{b}; }
	inline bool operator>=(const String& a, const String& b) noexcept { return std::string_view{a} >= std::string_view{b}; }
	inline bool operator<(const String& a, const String& b) noexcept { return std::string_view{a} < std::string_view{b}; }
	inline bool operator<=(const String& a, const String& b) noexcept { return std::string_view{a} <= std::string_view{b}; }
}

#endif
