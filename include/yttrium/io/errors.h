/// \file
/// \brief

#ifndef _include_yttrium_io_errors_h_
#define _include_yttrium_io_errors_h_

#include <yttrium/string.h>

#include <exception>

namespace Yttrium
{
	class ResourceError : public std::exception
	{
	public:
		ResourceError(Allocator& allocator) : _what(&allocator) {}
		const char* what() const noexcept override { return _what.text(); }

	private:
		String _what;

		template <typename T>
		friend ResourceError operator<<(ResourceError&&, T&&);

	public:
		ResourceError(const ResourceError&) = delete;
		ResourceError(ResourceError&&) = default;
		ResourceError& operator=(const ResourceError&) = delete;
		ResourceError& operator=(ResourceError&&) = default;
	};

	template <typename T>
	ResourceError operator<<(ResourceError&& e, T&& value)
	{
		e._what << std::forward<T>(value);
		return std::move(e);
	}
}

#endif
