// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <functional>
#include <memory>

namespace seir
{
	class Image;
}

namespace Yt
{
	class RenderManager;
	class RenderMetrics;
	class RenderPass;
	class Window;

	///
	class Viewport
	{
	public:
		explicit Viewport(Window&);
		~Viewport() noexcept;

		RenderMetrics metrics() const noexcept;

		///
		void render(const std::function<void(RenderPass&)>&);

		///
		RenderManager& render_manager();

		///
		seir::Image take_screenshot();

	private:
		const std::unique_ptr<struct ViewportData> _data;
		friend class Renderer2D;
	};
}
