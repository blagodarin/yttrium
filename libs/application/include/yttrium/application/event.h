// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/application/key.h>
#include <yttrium/base/flags.h>

#include <string_view>

namespace Yt
{
	class Window;

	/// %Key event.
	class KeyEvent
	{
	public:
		enum class Modifier
		{
			Shift = 1 << 0,   ///< Key::LShift or Key::RShift is pressed.
			Control = 1 << 1, ///< Key::LControl or Key::RControl is pressed.
			Alt = 1 << 2,     ///< Key::LAlt or Key::RAlt is pressed.
		};

		Key _key;                   ///< Key.
		bool _pressed;              ///< Is the key pressed?
		bool _autorepeat;           ///< Is it an autorepeat event?
		Flags<Modifier> _modifiers; ///< Modifier mask.

		///
		constexpr KeyEvent(Key key, bool pressed, bool autorepeat) noexcept
			: _key{ key }, _pressed{ pressed }, _autorepeat{ autorepeat } {}
	};

	class EventCallbacks
	{
	public:
		virtual ~EventCallbacks() noexcept = default;
		virtual void onWindowMouseMove(const Window&, int dx, int dy) = 0;
		virtual void onWindowKeyEvent(const Window&, const KeyEvent&) = 0;
		virtual void onWindowTextInput(const Window&, std::string_view) = 0;
	};
}
