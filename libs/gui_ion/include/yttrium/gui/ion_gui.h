// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string>

namespace Yt
{
	class AudioManager;
	enum class Key;
	class KeyEvent;
	class RectF;
	class RenderPass;
	class ResourceLoader;
	class ScriptContext;
	class Vector2;

	class IonGuiCanvas
	{
	public:
		virtual ~IonGuiCanvas() = default;
		virtual void on_draw(RenderPass&, const RectF&, std::chrono::milliseconds) {}
		virtual void on_mouse_move(const RectF&, const Vector2&) {}
		virtual bool on_mouse_press(const RectF&, Key, const Vector2&) { return false; }
	};

	class IonGui
	{
	public:
		IonGui(std::string_view name, ResourceLoader&, ScriptContext&, const std::shared_ptr<AudioManager>& = {});
		~IonGui();

		void bind_canvas(const std::string& name, IonGuiCanvas&);
		void draw(RenderPass&, const Vector2& cursor) const;
		const std::string& icon_path() const;
		void notify(const std::string& event);
		void on_custom_cursor(const std::function<void(RenderPass&, const Vector2&)>&);
		void on_quit(const std::function<void()>&);
		bool process_key_event(const KeyEvent&);
		bool process_text_input(std::string_view text);
		void start();
		const std::string& title() const;

	private:
		const std::unique_ptr<class IonGuiPrivate> _private;
	};
}
