// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/renderer/viewport.h>

#include <yttrium/application/id.h>
#include <yttrium/application/window.h>
#include <yttrium/image/image.h>
#include <yttrium/renderer/modifiers.h>
#include "builtin.h"
#include "pass.h"
#include "renderer.h"

namespace Yt
{
	class ViewportPrivate
	{
	public:
		Window& _window;
		Size _window_size;
		RendererImpl _renderer{ _window.id() };
		RenderBuiltin _renderer_builtin{ *_renderer._backend };
		RenderPassData _render_pass_data;
		bool _take_screenshot = false;
		std::function<void(RenderPass&, const Vector2&, const RenderReport&)> _on_render;
		std::function<void(Image&&)> _on_screenshot;

		explicit ViewportPrivate(Window& window)
			: _window{ window } {}
	};

	Viewport::Viewport(Window& window)
		: _private{ std::make_unique<ViewportPrivate>(window) }
	{
	}

	Viewport::~Viewport() noexcept = default;

	void Viewport::on_render(const std::function<void(RenderPass&, const Vector2&, const RenderReport&)>& callback)
	{
		_private->_on_render = callback;
	}

	void Viewport::on_screenshot(const std::function<void(Image&&)>& callback)
	{
		_private->_on_screenshot = callback;
	}

	void Viewport::render(RenderReport& current, const RenderReport& previous)
	{
		const auto window_size = _private->_window.size();
		if (window_size != _private->_window_size)
		{
			_private->_renderer.set_window_size(window_size);
			_private->_window_size = window_size;
		}
		{
			RenderPassImpl pass{ *_private->_renderer._backend, _private->_renderer_builtin, _private->_render_pass_data, window_size, current };
			PushProgram program{ pass, _private->_renderer_builtin._program_2d.get() };
			Push2D projection{ pass };
			if (_private->_on_render)
				_private->_on_render(pass, Vector2{ _private->_window.cursor() }, previous);
			pass.draw_debug_text();
		}
		_private->_window.swap_buffers();
		if (_private->_take_screenshot)
		{
			_private->_take_screenshot = false;
			if (_private->_on_screenshot)
				_private->_on_screenshot(_private->_renderer.take_screenshot(window_size));
		}
	}

	RenderManager& Viewport::render_manager()
	{
		return _private->_renderer;
	}

	void Viewport::take_screenshot()
	{
		_private->_take_screenshot = true;
	}
}
