// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/application/key.h>

#include <optional>
#include <string_view>

namespace Yt
{
	class Size;

	class WindowBackendCallbacks
	{
	public:
		virtual ~WindowBackendCallbacks() = default;
		virtual void on_focus_event(bool is_focused) = 0;
		virtual void on_key_event(Key, bool pressed, bool autorepeat, Flags<KeyEvent::Modifier>) = 0;
		virtual void on_resize_event(const Size&) = 0;
		virtual void on_text_input(std::string_view) = 0;
	};
}
