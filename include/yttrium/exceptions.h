/// \file
/// \brief

#ifndef _include_yttrium_io_errors_h_
#define _include_yttrium_io_errors_h_

#include <yttrium/string.h>
#include <yttrium/string_format.h>

#include <exception>

namespace Yttrium
{
	// TODO: Fix.
	class InitializationError;
	class ResourceError;

	///
	class Exception : public std::exception
	{
	public:
		Exception(Allocator& allocator) : _what(&allocator) {}
		const char* what() const noexcept override { return _what.text(); }

	private:
		String _what;
		template <typename T> friend Exception operator<<(Exception&&, T&&);
		template <typename T> friend InitializationError operator<<(InitializationError&&, T&&);
		template <typename T> friend ResourceError operator<<(ResourceError&&, T&&);

	public:
		Exception(const Exception&) = delete;
		Exception(Exception&&) = default;
		Exception& operator=(const Exception&) = delete;
		Exception& operator=(Exception&&) = default;
	};

	template <typename T>
	Exception operator<<(Exception&& e, T&& value)
	{
		e._what << std::forward<T>(value);
		return std::move(e);
	}

	/// Thrown if a subsystem failed to initialize.
	/// This usually means inability to use the corresponding subsystem.
	class InitializationError : public Exception
	{
	public:
		InitializationError(Allocator& allocator) : Exception(allocator) {}
	};

	template <typename T>
	InitializationError operator<<(InitializationError&& e, T&& value)
	{
		e._what << std::forward<T>(value);
		return std::move(e);
	}

	///
	class ResourceError : public Exception
	{
	public:
		ResourceError(Allocator& allocator) : Exception(allocator) {}
	};

	template <typename T>
	ResourceError operator<<(ResourceError&& e, T&& value)
	{
		e._what << std::forward<T>(value);
		return std::move(e);
	}
}

#endif
