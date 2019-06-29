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

#include "../../../../core/utils/memory.h"

#include <GL/glx.h>

namespace Yttrium
{
	class GlxContext
	{
	public:
		struct Extensions
		{
			bool EXT_swap_control = false;
			void (*SwapIntervalEXT)(::Display*, ::GLXDrawable, int) = nullptr;

			bool EXT_swap_control_tear = false;
		};

		GlxContext(::Display*, int screen);
		~GlxContext();

		void bind(::Window) const;
		void swap_buffers(::Window) const;
		void unbind() const;
		const ::XVisualInfo* visual_info() const { return _visual_info.get(); }

		const Extensions* operator->() const { return &_extensions; }

	private:
		::Display* const _display;
		const int _screen;
		UniquePtr<::XVisualInfo, ::XFree> _visual_info;
		::GLXContext _context = nullptr;
		Extensions _extensions;
	};
}
