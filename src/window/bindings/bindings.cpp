#include <yttrium/bindings.h>

#include <yttrium/script/context.h>
#include "lookup.h"

#include <array>

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
}

namespace Yttrium
{
	class BindingsPrivate
	{
	public:
		BindingsPrivate(ScriptContext& script_context)
			: _script_context(script_context)
		{
		}

		bool is_valid_index(size_t index) const
		{
			return bind_names[index][0] && !_actions[index].first.is_empty();
		}

	public:
		ScriptContext& _script_context;
		std::array<std::pair<String, ScriptCode>, KeyCount> _actions;
	};

	std::pair<StaticString, StaticString> Bindings::Iterator::operator*() const
	{
		return std::make_pair(bind_names[_index], StaticString(_bindings._private->_actions[_index].first));
	}

	void Bindings::Iterator::operator++()
	{
		if (_index == _bindings._private->_actions.size())
			return;
		do
		{
			++_index;
			if (_bindings._private->is_valid_index(_index))
				break;
		} while (_index < _bindings._private->_actions.size());
	}

	Bindings::Bindings(ScriptContext& script_context, Allocator& allocator)
		: _private(make_unique<BindingsPrivate>(allocator, script_context))
	{
	}

	Bindings::Iterator Bindings::begin() const
	{
		size_t index = 0;
		while (index < _private->_actions.size() && !_private->is_valid_index(index))
			++index;
		return Iterator(*this, index);
	}

	void Bindings::bind(Key key, const StaticString& action)
	{
		auto& binding = _private->_actions[KeyType(key)];
		binding.first.swap(String(action, &_private.allocator()));
		binding.second = ScriptCode(binding.first, _private->_script_context.allocator());
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
		auto& binding = _private->_actions[KeyType(key)];
		if (binding.first.is_empty())
		{
			binding.first.swap(String(action, &_private.allocator()));
			binding.second = ScriptCode(binding.first, _private->_script_context.allocator());
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

	bool Bindings::call(Key key, ScriptCodeMode mode)
	{
		const auto& binding = _private->_actions[KeyType(key)];
		if (binding.first.is_empty())
			return false;
		binding.second.execute(_private->_script_context, mode);
		return true;
	}

	void Bindings::clear()
	{
		for (auto& binding : _private->_actions)
		{
			binding.first.clear();
			binding.second = {};
		}
	}

	Bindings::Iterator Bindings::end() const
	{
		return Iterator(*this, _private->_actions.size());
	}

	void Bindings::unbind(Key key)
	{
		auto& binding = _private->_actions[KeyType(key)];
		binding.first.clear();
		binding.second = {};
	}

	bool Bindings::unbind(const StaticString& name)
	{
		const Key key = lookup_key(name);
		if (key == Key::Null)
			return false;
		unbind(key);
		return true;
	}

	Bindings::~Bindings() = default;
}
