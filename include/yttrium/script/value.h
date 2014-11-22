/// \file
/// \brief Script value.

#ifndef __Y_SCRIPT_VALUE_H
#define __Y_SCRIPT_VALUE_H

#include <yttrium/string.h>

namespace Yttrium
{

class ScriptArgs;
class ScriptCode;
class ScriptContext;
class ScriptParser;

/// Script value data.
class Y_API ScriptValue
{
	friend ScriptArgs;
	friend ScriptCode;
	friend ScriptContext;
	friend ScriptParser;

public:

	enum
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
	int32_t to_int32() const noexcept { return _value.to_int32(); }

	///
	double to_double() const noexcept { return _value.to_double(); }

	///
	Yttrium::String to_string() const noexcept { return _value; }

	///
	Type type() const noexcept { return _type; }

public:

	///
	ScriptValue& operator=(int32_t value) noexcept;

	///

	ScriptValue& operator=(double value) noexcept;

	///

	ScriptValue& operator=(const StaticString& value) noexcept;

private:

	Type            _type;
	Yttrium::String _value;

private:

	Y_PRIVATE ScriptValue(int32_t value, Allocator* allocator);
	Y_PRIVATE ScriptValue(double value, Allocator* allocator);
	Y_PRIVATE ScriptValue(const StaticString& value, Allocator* allocator);
	Y_PRIVATE ScriptValue(const StaticString& value, Type type, Allocator* allocator);
};

} // namespace Yttrium

#endif // __Y_SCRIPT_VALUE_H
