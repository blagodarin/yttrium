#ifndef _include_yttrium_utils_tiny_string_map_h_
#define _include_yttrium_utils_tiny_string_map_h_

#include <yttrium/api.h>

#include <memory>
#include <string_view>

namespace Yttrium
{
	///
	class Y_CORE_API TinyStringMap
	{
	public:
		TinyStringMap() noexcept = default;
		TinyStringMap(const TinyStringMap&);
		TinyStringMap(TinyStringMap&&) noexcept;
		TinyStringMap& operator=(const TinyStringMap&);
		TinyStringMap& operator=(TinyStringMap&&) noexcept;

		/// Removes the element with the specified key.
		void erase(std::string_view key) noexcept;

		///
		std::string_view find(std::string_view key) const noexcept;

		///
		void insert_or_assign(std::string_view key, std::string_view value);

		/// Reserves the specified number of bytes for the data buffer.
		void reserve_bytes(size_t);

	private:
		std::unique_ptr<uint8_t[]> _data;
		size_t _capacity = 0;
		size_t _size = 0;
	};
}

#endif
