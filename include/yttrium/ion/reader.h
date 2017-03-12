/// \file
/// \brief

#ifndef _include_yttrium_ion_reader_h_
#define _include_yttrium_ion_reader_h_

#include <yttrium/static_string.h>
#include <yttrium/string_utils.h>

#include <memory>
#include <stdexcept>

namespace Yttrium
{
	class Reader;

	///
	class IonError : public std::runtime_error
	{
	public:
		///
		template <typename... Args>
		IonError(std::size_t line, std::ptrdiff_t column, Args&&... args)
			: std::runtime_error{make_string("(", line, ":", column, ") ", std::forward<Args>(args)...).c_str()}, _line{line}, _column{column} {}

		std::ptrdiff_t column() const noexcept { return _column; }
		std::size_t line() const noexcept { return _line; }

	protected:
		std::size_t _line;
		std::ptrdiff_t _column;
	};

	///
	class Y_API IonReader
	{
	public:
		struct Token
		{
			enum class Type
			{
				Name,
				Value,
				ListBegin,
				ListEnd,
				ObjectBegin,
				ObjectEnd,
				End,
			};

			Token(std::size_t line, std::ptrdiff_t column, Type type, const StaticString& text) noexcept
				: _line(line), _column(column), _type(type), _text(text) {}

			std::ptrdiff_t column() const noexcept { return _column; }
			std::size_t line() const noexcept { return _line; }
			StaticString text() const noexcept { return _text; }
			Type type() const noexcept { return _type; }

			void check_end() const;
			void check_list_begin() const;
			void check_name(const StaticString&) const;
			StaticString to_name() const;
			StaticString to_value() const;

		private:
			std::size_t _line;
			std::ptrdiff_t _column;
			Type _type;
			StaticString _text;
		};

		///
		explicit IonReader(const Reader&);

		///
		~IonReader();

		///
		Token read();

	private:
		const std::unique_ptr<class IonReaderPrivate> _private;
	};
}

#endif
