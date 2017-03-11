#include <yttrium/ion/reader.h>

#include <yttrium/memory/buffer.h>
#include <yttrium/storage/reader.h>
#include <yttrium/string_utils.h>

#include <array>
#include <vector>

namespace
{
	enum CharClass
	{
		Other,    // Any character not mentioned below.
		End,      // '\0'.
		Space,    // '\t', '\v', '\f' or ' '.
		Cr,       // '\r'.
		Lf,       // '\n'.
		Name,     // 'A'-'Z', 'a'-'z', '0'-'9' or '_'.
		Quote,    // '"'.
		LBrace,   // '{'.
		RBrace,   // '}'.
		LBracket, // '['.
		RBracket, // ']'.
		Comment,  // '#'.
	};

	const std::array<CharClass, 256> char_class =
	{
		// #0 - #31: Special character set.

		End,    Other,  Other, Other,    Other, Other,    Other, Other, // \0
		Other,  Space,  Lf,    Space,    Space, Cr,       Other, Other, // \t \n \v \f \r
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //

		// #32 - #127: Basic character set.

		Space,  Other,  Quote, Comment,  Other, Other,    Other, Other, //   ! " # $ % & '
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, // ( ) * + , - . /
		Name,   Name,   Name,  Name,     Name,  Name,     Name,  Name,  // 0 1 2 3 4 5 6 7
		Name,   Name,   Other, Other,    Other, Other,    Other, Other, // 8 9 : ; < = > ?
		Other,  Name,   Name,  Name,     Name,  Name,     Name,  Name,  // @ A B C D E F G
		Name,   Name,   Name,  Name,     Name,  Name,     Name,  Name,  // H I J K L M N O
		Name,   Name,   Name,  Name,     Name,  Name,     Name,  Name,  // P Q R S T U V W
		Name,   Name,   Name,  LBracket, Other, RBracket, Other, Name,  // X Y Z [ \ ] ^ _
		Other,  Name,   Name,  Name,     Name,  Name,     Name,  Name,  // ` a b c d e f g
		Name,   Name,   Name,  Name,     Name,  Name,     Name,  Name,  // h i j k l m n o
		Name,   Name,   Name,  Name,     Name,  Name,     Name,  Name,  // p q r s t u v w
		Name,   Name,   Name,  LBrace,   Other, RBrace,   Other, Other, // x y z { | } ~

		// #128 - #255: Extended character set.

		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
	};

	CharClass class_of(char c)
	{
		return ::char_class[static_cast<unsigned char>(c)];
	}
}

namespace Yttrium
{
	StaticString IonReader::Token::to_name() const
	{
		if (_type != Type::Name)
			throw IonError{_line, _column, "ION name expected"};
		return _text;
	}

	class IonReaderPrivate
	{
	public:
		IonReaderPrivate(const Reader& reader)
			: _buffer{read_buffer(reader)}
		{
		}

		IonReader::Token read()
		{
			for (;;)
			{
				switch (::class_of(*_cursor))
				{
				case Other:
					throw IonError{_line, _cursor - _line_base, "Bad character"};

				case End:
					if (_cursor != static_cast<const char*>(_buffer.data()) + _buffer.size())
						throw IonError{_line, _cursor - _line_base, "Bad character"};
					return make_token<IonReader::Token::Type::End>(_cursor, 0);

				case Space:
					do { ++_cursor; } while (::class_of(*_cursor) == Space);
					break;

				case Cr:
					if (_cursor[1] == '\n')
						++_cursor;
				case Lf:
					_line_base = _cursor++;
					++_line;
					break;

				case Name:
					if (!(_stack.back() & AcceptNames))
						throw IonError{_line, _cursor - _line_base, "Unexpected ION name"};
					{
						const auto base = _cursor;
						do { ++_cursor; } while (::class_of(*_cursor) == Name);
						_stack.back() |= AcceptValues;
						return make_token<IonReader::Token::Type::Name>(base, _cursor - base);
					}

				case Quote:
					if (!(_stack.back() & AcceptValues))
						throw IonError{_line, _cursor - _line_base, "Unexpected ION value"};
					{
						auto cursor = _cursor;
						const auto base = ++cursor;
						while (*cursor != '"')
						{
							switch (*cursor)
							{
							case '\0':
								throw IonError{_line, cursor - _line_base, cursor == static_cast<const char*>(_buffer.data()) + _buffer.size() ? "Unexpected end of file" : "Bad character"};
							case '\n':
							case '\r':
								throw IonError{_line, cursor - _line_base, "Unexpected end of line"};
							default:
								++cursor;
							}
						}
						_cursor = cursor + 1;
						return make_token<IonReader::Token::Type::Value, -1>(base, cursor - base);
					}

				case LBrace:
					if (!(_stack.back() & AcceptValues))
						throw IonError{_line, _cursor - _line_base, "Unexpected list"};
					_stack.emplace_back(AcceptNames);
					return make_token<IonReader::Token::Type::ObjectBegin>(_cursor++, 1);

				case RBrace:
					if (!(_stack.back() & AcceptNames) || _stack.size() == 1)
						throw IonError{_line, _cursor - _line_base, "Unexpected end of object"};
					_stack.pop_back();
					return make_token<IonReader::Token::Type::ObjectEnd>(_cursor++, 1);

				case LBracket:
					if (!(_stack.back() & AcceptValues))
						throw IonError{_line, _cursor - _line_base, "Unexpected list"};
					_stack.emplace_back(AcceptValues);
					return make_token<IonReader::Token::Type::ListBegin>(_cursor++, 1);

				case RBracket:
					if ((_stack.back() & (AcceptNames | AcceptValues)) != AcceptValues)
						throw IonError{_line, _cursor - _line_base, "Unexpected end of object"};
					_stack.pop_back();
					return make_token<IonReader::Token::Type::ListEnd>(_cursor++, 1);

				case Comment:
					do { ++_cursor; } while (*_cursor != '\n' && *_cursor != '\r' && *_cursor != '\0');
					break;
				}
			}
		}

	private:
		template <IonReader::Token::Type type, std::ptrdiff_t column_offset = 0>
		IonReader::Token make_token(const char* begin, std::ptrdiff_t size) noexcept
		{
			return {_line, begin - _line_base + column_offset, type, {begin, static_cast<std::size_t>(size)}};
		}

		static Buffer read_buffer(const Reader& reader)
		{
			Buffer buffer;
			if (!reader)
			{
				buffer.resize(1);
				buffer[0] = '\0';
				buffer.resize(0);
			}
			else if (!reader.read_all(buffer))
				throw std::runtime_error{"IO error"}; // TODO: Throw some actual IO error exception.
			return buffer;
		}

	private:
		enum
		{
			AcceptNames = 1 << 0,
			AcceptValues = 1 << 1,
		};

		const Buffer _buffer;
		const char* _cursor{static_cast<const char*>(_buffer.data())};
		std::size_t _line{1};
		const char* _line_base{_cursor - 1};
		std::vector<uint8_t> _stack{AcceptNames};
	};

	IonReader::IonReader(const Reader& reader)
		: _private{std::make_unique<IonReaderPrivate>(reader)}
	{
	}

	IonReader::~IonReader() = default;

	IonReader::Token IonReader::read()
	{
		return _private->read();
	}

	bool read_name(IonReader& ion, const StaticString& name)
	{
		const auto token = ion.read();
		if (token.type() == IonReader::Token::Type::End)
			return false;
		if (token.to_name() != name)
			throw IonError{token.line(), token.column(), "'", name, "' expected"};
		return true;
	}

	StaticString read_value(IonReader& ion)
	{
		const auto token = ion.read();
		if (token.type() != IonReader::Token::Type::Value)
			throw IonError{token.line(), token.column(), "ION value expected"};
		return token.text();
	}
}
