/// \file
/// \brief

#ifndef _include_yttrium_tiny_string_map_h_
#define _include_yttrium_tiny_string_map_h_

#include <yttrium/static_string.h>

#include <memory>

namespace Yttrium
{
	///
	class Y_API TinyStringMap
	{
	public:
		TinyStringMap() noexcept = default;
		TinyStringMap(const TinyStringMap&);
		TinyStringMap(TinyStringMap&&) noexcept = default;
		TinyStringMap& operator=(const TinyStringMap&);
		TinyStringMap& operator=(TinyStringMap&&) noexcept = default;

		/// Removes the element with the specified key if there is one.
		void erase(const StaticString& key) noexcept;

		///
		StaticString find(const StaticString& key) const noexcept;

		///
		void insert_or_assign(const StaticString& key, const StaticString& value);

		/// Reserves the specified number of bytes for the data buffer.
		void reserve_bytes(size_t);

	private:
		std::unique_ptr<uint8_t[]> _data;
		size_t _capacity = 0;
		size_t _size = 0;
	};
}

#endif
