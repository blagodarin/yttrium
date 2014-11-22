#include <yttrium/bindings.h>

#include "lookup.h"

namespace Yttrium
{

namespace
{

const char* bind_names[] =
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
	"f13",
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

Bindings::Bindings(Allocator* allocator)
	: _allocator(allocator)
{
}

void Bindings::bind(Key key, const StaticString& action)
{
	auto& binding = _actions[KeyType(key)];
	binding.first.swap(String(action, _allocator));
	binding.second = ScriptCode(binding.first);
}

bool Bindings::bind(const StaticString& name, const StaticString& action)
{
	const Key key = lookup_key(name);
	if (key == Key::Null)
		return false;
	bind(key, action);
	return true;
}

void Bindings::bind_default(Key key, const StaticString& action)
{
	auto& binding = _actions[KeyType(key)];
	if (binding.first.is_empty())
	{
		binding.first.swap(String(action, _allocator));
		binding.second = ScriptCode(binding.first);
	}
}

bool Bindings::bind_default(const StaticString& name, const StaticString& action)
{
	const Key key = lookup_key(name);
	if (key == Key::Null)
		return false;
	bind_default(key, action);
	return true;
}

bool Bindings::call(Key key, ScriptCode::ExecutionMode mode)
{
	const auto& binding = _actions[KeyType(key)];
	if (binding.first.is_empty())
		return false;
	binding.second.execute(nullptr, mode);
	return true;
}

void Bindings::clear()
{
	for (auto& binding: _actions)
	{
		binding.first.clear();
		binding.second = ScriptCode();
	}
}

std::map<String, String> Bindings::map() const
{
	std::map<String, String> result;
	for (size_t i = 0; i < _actions.size(); ++i)
	{
		if (bind_names[i][0] && !_actions[i].first.is_empty())
			result.emplace(String(bind_names[i], _allocator), _actions[i].first);
	}
	return result;
}

void Bindings::unbind(Key key)
{
	auto& binding = _actions[KeyType(key)];
	binding.first.clear();
	binding.second = ScriptCode();
}

bool Bindings::unbind(const StaticString& name)
{
	const Key key = lookup_key(name);
	if (key == Key::Null)
		return false;
	unbind(key);
	return true;
}

} // namespace Yttrium
