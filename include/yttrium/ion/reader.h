#ifndef _include_yttrium_ion_reader_h_
#define _include_yttrium_ion_reader_h_

#include <yttrium/exceptions.h>

#include <memory>
#include <stdexcept>

namespace Yttrium
{
	class Color4f;
	class Source;

	///
	class IonError : public DataError
	{
	public:
		///
		template <typename... Args>
		IonError(std::size_t line, std::ptrdiff_t column, Args&&... args) : DataError{"(", line, ":", column, ") ", std::forward<Args>(args)...} {}
	};

	///
	class Y_CORE_API IonReader
	{
	public:
		struct Y_CORE_API Token
		{
			enum class Type
			{
				Name,
				Value,
				Color,
				ListBegin,
				ListEnd,
				ObjectBegin,
				ObjectEnd,
				End,
			};

			Token(std::size_t line, std::ptrdiff_t column, Type type, std::string_view text, bool translatable = false) noexcept
				: _line{line}, _column{column}, _type{type}, _text{text}, _translatable{translatable} {}

			std::ptrdiff_t column() const noexcept { return _column; }
			std::size_t line() const noexcept { return _line; }
			std::string_view text() const noexcept { return _text; }
			bool translatable() const noexcept { return _translatable; }
			Type type() const noexcept { return _type; }

			void check_end() const;
			void check_list_begin() const;
			void check_list_end() const;
			void check_name(std::string_view) const;
			void check_object_begin() const;
			void check_object_end() const;
			Token& next(IonReader&);
			Color4f to_color() const;
			std::string_view to_name() const;
			std::string_view to_value() const;

		private:
			std::size_t _line;
			std::ptrdiff_t _column;
			Type _type;
			std::string_view _text;
			bool _translatable;
		};

		explicit IonReader(const Source&);
		~IonReader();

		///
		Token read();

	private:
		const std::unique_ptr<class IonReaderPrivate> _private;
	};
}

#endif
