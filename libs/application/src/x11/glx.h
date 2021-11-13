// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <seir_base/pointer.hpp>

#include <GL/glx.h>
#ifdef None
#	undef None
constexpr long None = 0;
#endif

namespace Yt
{
	class GlxContext
	{
	public:
		struct Extensions
		{
			bool EXT_swap_control = false;
			void (*SwapIntervalEXT)(::Display*, GLXDrawable, int) = nullptr;

			bool EXT_swap_control_tear = false;
		};

		GlxContext(::Display*, int screen);
		~GlxContext();

		void bind(::Window) const;
		void swap_buffers(::Window) const;
		void unbind() const;
		const XVisualInfo* visual_info() const { return _visual_info; }

		const Extensions* operator->() const { return &_extensions; }

	private:
		::Display* const _display;
		const int _screen;
		seir::CPtr<XVisualInfo, ::XFree> _visual_info;
		GLXContext _context = nullptr;
		Extensions _extensions;
	};
}
