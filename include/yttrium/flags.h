/// \file
/// \brief

#ifndef _include_yttrium_flags_h_
#define _include_yttrium_flags_h_

#include <type_traits>

namespace Yttrium
{
	template <typename T>
	class Flags
	{
	public:
		Flags() = default;
		constexpr Flags(const T flag) : _flags(static_cast<std::underlying_type_t<T>>(flag)) {}
		constexpr explicit operator std::underlying_type_t<T>() const { return _flags; }
		constexpr Flags& operator|=(const Flags other) { _flags |= other._flags; return *this; }
	private:
		std::underlying_type_t<T> _flags = 0;
	};

	template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
	constexpr bool operator==(const Flags<T> a, const Flags<T> b)
	{
		using U = std::underlying_type_t<T>;
		return static_cast<U>(a) == static_cast<U>(b);
	}

	template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
	constexpr bool operator!=(const Flags<T> a, const Flags<T> b)
	{
		return !(a == b);
	}

	template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
	constexpr auto operator&(const Flags<T> a, const T b)
	{
		using U = std::underlying_type_t<T>;
		return static_cast<U>(a) & static_cast<U>(b);
	}

	template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
	constexpr auto operator&(const T a, const Flags<T> b)
	{
		using U = std::underlying_type_t<T>;
		return static_cast<U>(a) & static_cast<U>(b);
	}

	template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
	constexpr auto operator&(const Flags<T> a, const Flags<T> b)
	{
		using U = std::underlying_type_t<T>;
		return static_cast<U>(a) & static_cast<U>(b);
	}

	template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
	constexpr Flags<T> operator|(const Flags<T> a, const Flags<T> b)
	{
		using U = std::underlying_type_t<T>;
		return static_cast<T>(static_cast<U>(a) | static_cast<U>(b));
	}

	inline namespace Operators
	{
		template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
		constexpr Flags<T> operator|(const T a, const T b)
		{
			using U = std::underlying_type_t<T>;
			return static_cast<T>(static_cast<U>(a) | static_cast<U>(b));
		}
	}
}

#endif
