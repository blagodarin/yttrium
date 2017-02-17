/// \file
/// \brief

#ifndef _include_yttrium_exceptions_h_
#define _include_yttrium_exceptions_h_

#include <yttrium/static_string.h>
#include <yttrium/string_utils.h>

#include <exception>

namespace Yttrium
{
	///
	class Exception : public std::exception
	{
	public:
		///
		template <typename... Args>
		Exception(Args&&... args) { append_to(_what, std::forward<Args>(args)...); }

		const char* what() const noexcept override { return _what.c_str(); }

		Exception(const Exception&) = delete;
		Exception(Exception&&) = default;
		Exception& operator=(const Exception&) = delete;
		Exception& operator=(Exception&&) = default;

	protected:
		std::string _what;
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

	///
	class GuiDataError : public DataError
	{
	public:
		template <typename... Args>
		GuiDataError(Args&&... args) : DataError(std::forward<Args>(args)...) {}

		void set_source(const StaticString& source)
		{
			if (!_has_source)
			{
				_what = make_string("("_s, source, ") "_s, _what);
				_has_source = true;
			}
		}

	private:
		bool _has_source = false;
	};

	/// Thrown when required data is missing.
	class MissingDataError : public RuntimeError
	{
	public:
		template <typename... Args>
		MissingDataError(Args&&... args) : RuntimeError(std::forward<Args>(args)...) {}
	};

	/// Thrown by ResourceLoader if it is unable to find the specified resource.
	class ResourceError : public MissingDataError
	{
	public:
		template <typename... Args>
		ResourceError(Args&&... args) : MissingDataError(std::forward<Args>(args)...) {}
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
