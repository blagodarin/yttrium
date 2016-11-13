/// \file
/// \brief

#ifndef _include_yttrium_small_string_map_h_
#define _include_yttrium_small_string_map_h_

#include <yttrium/static_string.h>

#include <memory>

namespace Yttrium
{
	///
	class Y_API SmallStringMap
	{
	public:
		SmallStringMap() noexcept = default;
		SmallStringMap(const SmallStringMap&);
		SmallStringMap(SmallStringMap&&) noexcept = default;
		SmallStringMap& operator=(const SmallStringMap& other) { return *this = SmallStringMap(other); }
		SmallStringMap& operator=(SmallStringMap&&) noexcept = default;

		///
		void erase(const StaticString& key) noexcept;

		///
		StaticString find(const StaticString& key) const noexcept;

		///
		void insert_or_assign(const StaticString& key, const StaticString& value);

	private:
		std::unique_ptr<uint8_t[]> _data;
		size_t _size = 0;
		size_t _capacity = 0;
	};
}

#endif
