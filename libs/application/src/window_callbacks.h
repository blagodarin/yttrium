// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/application/event.h>

#include <optional>
#include <string_view>

namespace seir
{
	class Size;
}

namespace Yt
{
	class WindowBackendCallbacks
	{
	public:
		virtual ~WindowBackendCallbacks() = default;
		virtual void on_focus_event(bool is_focused) = 0;
		virtual void on_key_event(Key, bool pressed, bool autorepeat, Flags<KeyEvent::Modifier>) = 0;
		virtual void on_resize_event(const seir::Size&) = 0;
		virtual void on_text_input(std::string_view) = 0;
	};
}
