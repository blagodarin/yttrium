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

#include <windows.h>

#include <GL/gl.h>
#include <GL/wglext.h>

namespace Yttrium
{
	class WglContext
	{
	public:
		struct Extensions
		{
			// clang-format off

			bool ARB_extensions_string = false;
			const char* (APIENTRY* GetExtensionsStringARB)(HDC) = nullptr;
			const char* EXTENSIONS_ARB = nullptr;

			bool EXT_swap_control = false;
			int (APIENTRY* GetSwapIntervalEXT)() = nullptr;
			BOOL (APIENTRY* SwapIntervalEXT) (int) = nullptr;

			// clang-format on
		};

		explicit WglContext(HDC);

		const Extensions* operator->() const { return &_wgl; }

	private:
		class RenderingContext
		{
		public:
			RenderingContext(HDC, HGLRC);
			~RenderingContext();
			operator HGLRC() const { return _hglrc; }
			static HGLRC create(HDC);

		private:
			const HGLRC _hglrc;
		};

	private:
		const RenderingContext _hglrc;
		Extensions _wgl;
	};
}
