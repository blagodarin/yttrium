#include <Yttrium/bindings.h>

#include <Yttrium/script/manager.h>

#include "lookup.h"

namespace Yttrium
{

namespace
{

const char *bind_names[] =
{
	// Null key (0x00):

	"",

	// Keyboard keys (0x01 - 0x7F):

	// - Special keys (0x01 - 0x0F);

	"lshift",
	"rshift",
	"lctrl",
	"rctrl",
	"lalt",
	"ralt",
	"up",
	"down",
	"left",
	"right",
	"",
	"",
	"print",
	"pause",
	"esc",

	// - Text-producing keys (0x10 - 0x3F);

	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"l",
	"m",
	"n",
	"o",
	"p",
	"q",
	"r",
	"s",
	"t",
	"u",
	"v",
	"w",
	"x",
	"y",
	"z",
	",",
	".",
	"semicolon",
	"'",
	"/",
	"\\",
	"[",
	"]",
	"-",
	"=",
	"`",
	"space",

	// Text-editing keys (0x40 - 0x4F);

	"tab",
	"enter",
	"back",
	"ins",
	"del",
	"home",
	"end",
	"pgup",
	"pgdn",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	// - Functional keys (0x50 - 0x6F);

	"f1",
	"f2",
	"f3",
	"f4",
	"f5",
	"f6",
	"f7",
	"f8",
	"f9",
	"f10",
	"f11",
	"f12",
	"f13", // NOTE: Keys kF13 to kF24 currently can't be bound.
	"f14",
	"f15",
	"f16",
	"f17",
	"f18",
	"f19",
	"f20",
	"f21",
	"f22",
	"f23",
	"f24",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	// - Numpad keys (0x70 - 0x7F).

	"num0",
	"num1",
	"num2",
	"num3",
	"num4",
	"num5",
	"num6",
	"num7",
	"num8",
	"num9",
	"num.",
	"num+",
	"num-",
	"num*",
	"num/",
	"numenter",

	// Non-keyboard keys (0x80 - 0xFF):

	// - Mouse keys (0x80 - 0x8F);

	"mouse1",
	"mouse2",
	"mouse3",
	"mouse4",
	"mouse5",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"mwup",
	"mwdown",
	"mwleft",
	"mwright",

	// - Unassigned keys (0x90 - 0xFF).
};

} // namespace

Bindings::Bindings(Allocator *allocator)
	: _allocator(allocator)
{
}

void Bindings::bind(Key key, const StaticString &action)
{
	_actions[KeyType(key)].swap(String(action, _allocator));
}

bool Bindings::bind(const StaticString &name, const StaticString &action)
{
	Key key = lookup_key(name);

	if (key == Key::Null)
	{
		return false;
	}

	_actions[KeyType(key)].swap(String(action, _allocator));
	return true;
}

void Bindings::bind_default(Key key, const StaticString &action)
{
	String &old_action = _actions[KeyType(key)];

	if (old_action.is_empty())
	{
		old_action.swap(String(action, _allocator));
	}
}

bool Bindings::bind_default(const StaticString &name, const StaticString &action)
{
	Key key = lookup_key(name);

	if (key == Key::Null)
	{
		return false;
	}

	String &old_action = _actions[KeyType(key)];

	if (old_action.is_empty())
	{
		old_action.swap(String(action, _allocator));
	}

	return true;
}

bool Bindings::call(Key key, ExecutionMode mode)
{
	// TODO: Pre-parse the actions to avoid script rescanning,
	// memory allocations and other nasty things on every call.

	const String &action = _actions[KeyType(key)];

	if (action.is_empty())
	{
		return false;
	}

	ScriptManager::instance()->root_context().execute(action, mode);
	return true;
}

void Bindings::clear()
{
	for (String &action: _actions)
	{
		action.clear();
	}
}

Bindings::Map Bindings::map() const
{
	Map result;

	for (size_t i = 0; i < KeyType(Key::__Count); ++i)
	{
		if (bind_names[i][0] && !_actions[i].is_empty())
		{
			result.insert(Map::value_type(String(bind_names[i], _allocator), _actions[i]));
		}
	}

	return result;
}

void Bindings::unbind(Key key)
{
	_actions[KeyType(key)].clear();
}

bool Bindings::unbind(const StaticString &name)
{
	Key key = lookup_key(name);

	if (key == Key::Null)
	{
		return false;
	}

	_actions[KeyType(key)].clear();
	return true;
}

} // namespace Yttrium
