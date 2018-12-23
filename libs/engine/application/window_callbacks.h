//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <yttrium/key.h>

#include <optional>
#include <string_view>

namespace Yttrium
{
	class Size;

	class WindowBackendCallbacks
	{
	public:
		virtual ~WindowBackendCallbacks() = default;
		virtual void on_focus_event(bool is_focused) = 0;
		virtual void on_key_event(Key, bool is_pressed, const std::optional<Flags<KeyEvent::Modifier>>&) = 0;
		virtual void on_resize_event(const Size&) = 0;
		virtual void on_text_input(std::string_view) = 0;
	};
}
