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
		void on_render(const std::function<void(RenderPass&, const Vector2& cursor, const RenderReport&)>&);

		///
		void on_screenshot(const std::function<void(Image&&)>&);

		///
		void render(RenderReport& current, const RenderReport& previous);

		///
		RenderManager& render_manager();

		///
		void take_screenshot();

	private:
		const std::unique_ptr<class ViewportPrivate> _private;
		friend ViewportPrivate;
	};
}
