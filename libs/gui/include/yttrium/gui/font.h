// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace seir
{
	class Blob;
	template <class>
	class SharedPtr;
}

namespace Yt
{
	class RectF;
	class RenderManager;
	class Renderer2D;
	class Source;
	class Texture2D;

	class Font
	{
	public:
		enum class Graphics
		{
			WhiteRect,
		};

		struct TextCapture
		{
			const size_t _cursorOffset;
			const size_t _selectionBegin;
			const size_t _selectionEnd;
			std::optional<float> _cursorPosition;
			std::optional<std::pair<float, float>> _selectionRange;

			constexpr TextCapture(size_t cursorOffset, size_t selectionOffset, size_t selectionSize) noexcept
				: _cursorOffset{ cursorOffset }, _selectionBegin{ selectionOffset }, _selectionEnd{ selectionOffset + selectionSize } {}
		};

		static std::shared_ptr<const Font> load(const seir::SharedPtr<seir::Blob>&, RenderManager&);

		virtual ~Font() noexcept = default;
		virtual void render(Renderer2D&, const RectF&, std::string_view) const = 0;
		virtual float textWidth(std::string_view, float fontSize, TextCapture* = nullptr) const = 0;
		virtual std::shared_ptr<const Texture2D> texture() const noexcept = 0;
		virtual RectF textureRect(Graphics) const noexcept = 0;
	};
}
