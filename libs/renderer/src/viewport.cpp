// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/renderer/viewport.h>
#include "viewport.h"

#include <yttrium/image/image.h>

namespace Yt
{
	Viewport::Viewport(Window& window)
		: _data{ std::make_unique<ViewportData>(window) }
	{
	}

	Viewport::~Viewport() noexcept = default;

	RenderMetrics Viewport::metrics() const noexcept
	{
		return _data->_metrics;
	}

	void Viewport::render(const std::function<void(RenderPass&)>& callback)
	{
		const auto window_size = _data->_window.size();
		if (window_size != _data->_window_size)
		{
			_data->_renderer.set_viewport_size(window_size);
			_data->_window_size = window_size;
		}
		_data->_metrics = RenderMetrics{};
		{
			RenderPassImpl pass{ *_data->_renderer._backend, _data->_renderer_builtin, _data->_render_pass_data, window_size, _data->_metrics };
			callback(pass);
		}
		_data->_window.swap_buffers();
	}

	RenderManager& Viewport::render_manager()
	{
		return _data->_renderer;
	}

	Image Viewport::take_screenshot()
	{
		return _data->_renderer.take_screenshot(_data->_window_size);
	}
}
