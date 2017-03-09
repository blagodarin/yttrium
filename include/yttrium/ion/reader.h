/// \file
/// \brief

#ifndef _include_yttrium_ion_reader_h_
#define _include_yttrium_ion_reader_h_

#include <yttrium/static_string.h>

#include <memory>

namespace Yttrium
{
	class StaticString;
	class Reader;

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

		private:
			const std::size_t _line;
			const std::ptrdiff_t _column;
			const Type _type;
			const StaticString _text;
		};

		///
		IonReader(const Reader&);

		///
		~IonReader();

		///
		Token read();

	private:
		const std::unique_ptr<class IonReaderPrivate> _private;
	};
}

#endif
