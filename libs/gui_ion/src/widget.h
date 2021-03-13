// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/application/key.h>
#include <yttrium/math/rect.h>
#include <yttrium/utils/flags.h>
#include "widget_data.h"

namespace Yt
{
	class GuiPropertyLoader;
	class IonGuiPrivate;
	class RenderPass;

	class Widget
	{
	public:
		enum class Flag
		{
			CanHaveFocus = 1 << 0,
		};

		virtual ~Widget() noexcept;

		Vector2 clamp(const Vector2&) const noexcept;
		Flags<Flag> flags() const { return _flags; }
		bool is_focused() const { return _is_focused; }
		RectF rect() const { return _data->_rect; }
		RectF render_rect() const { return _render_rect; }
		void set_focused(bool focused) { _is_focused = focused; }
		void set_render_rect(const RectF& rect) { _render_rect = rect; }
		bool track_hover() const noexcept { return _data->_track_hover; }

		void draw(RenderPass&, const RectF&, WidgetData::Style) const;
		virtual bool process_key(const KeyEvent&) { return false; }
		virtual bool process_text(std::string_view) { return false; }
		virtual void process_mouse_move(const Vector2&) {}
		virtual bool process_mouse_press(Key, const Vector2&) { return false; }
		virtual void process_mouse_release() {}

	protected:
		Widget(IonGuiPrivate&, std::unique_ptr<WidgetData>&&, Flags<Flag> = {});

		virtual void draw(RenderPass&, const RectF&, WidgetData::StyleData&) const = 0;

	protected:
		IonGuiPrivate& _gui;
		const std::unique_ptr<WidgetData> _data;

	private:
		const Flags<Flag> _flags;
		bool _is_focused = false;
		RectF _render_rect;
	};
}