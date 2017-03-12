/// \file
/// \brief

#ifndef _include_yttrium_exceptions_h_
#define _include_yttrium_exceptions_h_

#include <yttrium/string_utils.h>

#include <stdexcept>

namespace Yttrium
{
	/// Thrown if the initialization of an object (e.g. Window) fails.
	/// This usually means the inability to use the corresponding object.
	class InitializationError : public std::runtime_error
	{
	public:
		template <typename... Args>
		InitializationError(Args&&... args) : std::runtime_error{make_string(std::forward<Args>(args)...)} {}
	};

	/// Thrown if the requested operation is not supported by the implementation.
	class NotSupportedError : public std::runtime_error
	{
	public:
		template <typename... Args>
		NotSupportedError(Args&&... args) : std::runtime_error{make_string(std::forward<Args>(args)...)} {}
	};

	/// Thrown if a resource can't be loaded from the supplied data.
	class DataError : public std::runtime_error
	{
	public:
		template <typename... Args>
		DataError(Args&&... args) : std::runtime_error{make_string(std::forward<Args>(args)...)} {}
	};

	///
	class GuiDataError : public DataError
	{
	public:
		template <typename... Args>
		GuiDataError(Args&&... args) : DataError{std::forward<Args>(args)...} {}
	};

	/// Thrown when required data is missing.
	class MissingDataError : public std::runtime_error
	{
	public:
		template <typename... Args>
		MissingDataError(Args&&... args) : std::runtime_error{make_string(std::forward<Args>(args)...)} {}
	};

	/// Thrown by ResourceLoader if it is unable to find the specified resource.
	class ResourceError : public MissingDataError
	{
	public:
		template <typename... Args>
		ResourceError(Args&&... args) : MissingDataError{std::forward<Args>(args)...} {}
	};
}

#endif
