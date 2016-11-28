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

		Exception(const Exception&) = delete;
		Exception(Exception&&) = default;
		Exception& operator=(const Exception&) = delete;
		Exception& operator=(Exception&&) = default;

	private:
		template <typename T, typename... Args>
		void append(T&& value, Args&&... args) { _what << std::forward<T>(value); append(std::forward<Args>(args)...); }
		void append() {}

	private:
		String _what;
	};

	/// Thrown if the initialization of an object (e.g. Window) fails.
	/// This usually means the inability to use the corresponding object.
	class InitializationError : public Exception
	{
	public:
		template <typename... Args>
		InitializationError(Args&&... args) : Exception(std::forward<Args>(args)...) {}
	};

	/// Generic error that can occur in an initialized environment.
	class RuntimeError : public Exception
	{
	public:
		template <typename... Args>
		RuntimeError(Args&&... args) : Exception(std::forward<Args>(args)...) {}
	};

	/// Thrown if a resource can't be loaded from the supplied data.
	class DataError : public RuntimeError
	{
	public:
		template <typename... Args>
		DataError(Args&&... args) : RuntimeError(std::forward<Args>(args)...) {}
	};

	/// Thrown by ResourceLoader if it is unable to find the specified resource.
	class ResourceError : public RuntimeError
	{
	public:
		template <typename... Args>
		ResourceError(Args&&... args) : RuntimeError(std::forward<Args>(args)...) {}
	};

	/// Thrown by script classes on any errors during script execution.
	class ScriptError : public RuntimeError
	{
	public:
		template <typename... Args>
		ScriptError(Args&&... args) : RuntimeError(std::forward<Args>(args)...) {}
	};
}

#endif