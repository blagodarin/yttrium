// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <functional>
#include <memory>

namespace Yt
{
	class Image;
	class RenderManager;
	class RenderPass;
	struct RenderReport;
	class Vector2;
	class Window;

	///
	class Viewport
	{
	public:
		explicit Viewport(Window&);
		~Viewport() noexcept;

		///
		void render(RenderReport&, const std::function<void(RenderPass&)>&);

		///
		RenderManager& render_manager();

		///
		Image take_screenshot();

	private:
		const std::unique_ptr<class ViewportPrivate> _private;
		friend ViewportPrivate;
	};
}
