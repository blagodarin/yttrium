// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <windows.h>

#include <GL/gl.h>
#include <GL/wglext.h>

namespace Yt
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
