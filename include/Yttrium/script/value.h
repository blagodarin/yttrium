/// \file
/// \brief Script value.

#ifndef __Y_SCRIPT_VALUE_H
#define __Y_SCRIPT_VALUE_H

#include <Yttrium/noncopyable.h>
#include <Yttrium/string.h>

namespace Yttrium
{

class ScriptArgs;
class ScriptContext;
class ScriptParser;

/// Script value data.

class Y_API ScriptValue
{
	friend class ScriptArgs;
	friend class ScriptContext;
	friend class ScriptParser;

public:

	/// Value flags.

	typedef uint_fast8_t Flags;

	enum: Flags
	{
		Default  = 1 << 0, ///< Default value flag.
		Archived = 1 << 1, ///< Archived value flag.
	};

	/// Value type.
	/// \note Every Name is a Literal, and every Literal is a String, but not visa versa.

	enum Type
	{
		String,  ///< String value.
		Literal, ///< Literal value (e.g. integer).
		Name,    ///< Name (e.g. identifier).
	};

public:

	///

	Integer integer() const noexcept;

	///

	Real real() const noexcept;

	///

	Yttrium::String string() const noexcept;

	///

	Type type() const noexcept;

public:

	///

	ScriptValue &operator =(Integer value) noexcept;

	///

	ScriptValue &operator =(Real value) noexcept;

	///

	ScriptValue &operator =(const StaticString &value) noexcept;

private:

	Type            _type;
	Yttrium::String _value;

private:

	ScriptValue(Integer value, Allocator *allocator);

	ScriptValue(Real value, Allocator *allocator);

	ScriptValue(const StaticString &value, Allocator *allocator);

	ScriptValue(const StaticString &value, Type type, Allocator *allocator);
};

////////////////////////////////////////////////////////////////////////////////

inline Integer ScriptValue::integer() const noexcept
{
	return _value.to_int32();
}

///

inline Real ScriptValue::real() const noexcept
{
	return _value.to_double();
}

///

inline String ScriptValue::string() const noexcept
{
	return _value;
}

///

inline ScriptValue::Type ScriptValue::type() const noexcept
{
	return _type;
}

} // namespace Yttrium

#endif // __Y_SCRIPT_VALUE_H
