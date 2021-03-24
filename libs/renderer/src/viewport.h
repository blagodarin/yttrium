// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/application/window.h>
#include "builtin.h"
#include "pass.h"
#include "renderer.h"

namespace Yt
{
	struct ViewportData
	{
		Window& _window;
		Size _window_size;
		RendererImpl _renderer{ _window.id() };
		RenderBuiltin _renderer_builtin{ *_renderer._backend };
		RenderPassData _render_pass_data;

		explicit ViewportData(Window& window)
			: _window{ window } {}
	};
}
