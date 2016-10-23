/// \file
/// \brief

#ifndef _include_yttrium_io_errors_h_
#define _include_yttrium_io_errors_h_

#include <yttrium/string.h>
#include <yttrium/string_format.h>

#include <exception>

namespace Yttrium
{
	///
	class Exception : public std::exception
	{
	public:
		///
		template <typename... Args>
		Exception(Args&&... args) { append(std::forward<Args>(args)...); }

		const char* what() const noexcept override { return _what.text(); }

	public:
		Exception(const Exception&) = delete;
		Exception(Exception&&) = default;
		Exception& operator=(const Exception&) = delete;
		Exception& operator=(Exception&&) = default;

	private:
		template <typename T, typename... Args>
		void append(T&& value, Args&&... args) { _what << value; append(std::forward<Args>(args)...); }
		void append() {}

	private:
		String _what;
	};

	/// Thrown if a resource can't be loaded from the supplied data.
	class DataError : public Exception
	{
	public:
		template <typename... Args>
		DataError(Args&&... args) : Exception(std::forward<Args>(args)...) {}
	};

	/// Thrown if the initialization of an object (e.g. Window) fails.
	/// This usually means the inability to use the corresponding object.
	class InitializationError : public Exception
	{
	public:
		template <typename... Args>
		InitializationError(Args&&... args) : Exception(std::forward<Args>(args)...) {}
	};

	/// Thrown by ResourceLoader if it is unable to find the specified resource.
	class ResourceError : public Exception
	{
	public:
		template <typename... Args>
		ResourceError(Args&&... args) : Exception(std::forward<Args>(args)...) {}
	};
}

#endif
