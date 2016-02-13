#ifndef _src_gui_exceptions_h_
#define _src_gui_exceptions_h_

#include <yttrium/string.h>
#include <yttrium/string_format.h>

#include <exception>
#include <utility>

namespace Yttrium
{
	class GuiError : public std::exception
	{
	public:
		GuiError(Allocator& allocator)
			: _what(&allocator)
			, _source_name(&allocator)
		{
		}

		const char* what() const noexcept override
		{
			return _what.text();
		}

		const StaticString source_name() const noexcept
		{
			return _source_name;
		}

		void set_file(const StaticString& source_name)
		{
			if (_source_name.is_empty())
				_source_name = source_name;
		}

		GuiError(const GuiError&) = delete;
		GuiError(GuiError&&) = default;
		GuiError& operator=(const GuiError&) = delete;
		GuiError& operator=(GuiError&&) = default;

	private:
		String _what;
		String _source_name;

		template <typename T>
		friend GuiError&& operator<<(GuiError&&, T&&);
	};

	template <typename T>
	GuiError&& operator<<(GuiError&& e, T&& value)
	{
		e._what << std::forward<T>(value);
		return std::move(e);
	}
}

#endif
