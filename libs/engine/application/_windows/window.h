//
// Copyright 2019 Sergei Blagodarin
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

#include <yttrium/math/size.h>
#include "application.h"

#if Y_RENDERER_OPENGL
#	include "wgl.h"
#endif

#include <optional>
#include <string>

namespace Yttrium
{
	class Image;
	class WindowBackendCallbacks;

	class WindowBackend : private NativeWindowCallbacks
	{
	public:
		WindowBackend(WindowBackendCallbacks&);

		void close();
		bool get_cursor(Point&);
		bool process_events() { return _application.process_events(); }
		bool set_cursor(const Point&);
		void set_icon(const Image&);
		void set_title(const std::string&);
		void show();
		std::optional<Size> size() const { return _size; }
		void swap_buffers();

	private:
		void on_close() override;
		void on_focus(bool) override;
		void on_key(Key, bool pressed, bool autorepeat, Flags<KeyEvent::Modifier>) override;
		void on_resize(const Size&) override;
		void on_text(std::string_view) override;

	private:
		class WindowDC
		{
		public:
			explicit WindowDC(HWND);
			operator HDC() const noexcept { return _hdc; }

		private:
			const HDC _hdc;
		};

		WindowBackendCallbacks& _callbacks;
		NativeApplication _application;
		NativeWindow _hwnd = _application.create_window(*this);
		const WindowDC _hdc{ _hwnd };
#if Y_RENDERER_OPENGL
		const WglContext _wgl{ _hdc };
#endif
		Size _size;
	};
}
