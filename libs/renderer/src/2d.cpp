// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/renderer/2d.h>

#include <yttrium/base/buffer.h>
#include <yttrium/geometry/margins.h>
#include <yttrium/geometry/quad.h>
#include <yttrium/geometry/rect.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/program.h>
#include <yttrium/renderer/viewport.h>
#include "2d.h"
#include "texture.h"
#include "viewport.h"

#include <seir_math/mat.hpp>

#include <cassert>

namespace Yt
{
	struct Renderer2DData
	{
		struct Part
		{
			std::shared_ptr<const Texture2D> _texture;
			Buffer _vertices;
			Buffer _indices;

			explicit Part(const std::shared_ptr<const Texture2D>& texture) noexcept
				: _texture{ texture } {}
		};

		const ViewportData& _viewportData;
		std::vector<Part> _parts;
		Part* _currentPart = nullptr;
		seir::Rgba32 _color = seir::Rgba32::white();
		RectF _textureRect;
		MarginsF _textureBorders;

		struct Batch
		{
			Vertex2D* _vertices = nullptr;
			uint16_t* _indices = nullptr;
			size_t _baseIndex = 0;
		};

		explicit Renderer2DData(const ViewportData& viewportData)
			: _viewportData{ viewportData }
			, _currentPart{ &_parts.emplace_back(_viewportData._renderer_builtin._white_texture) }
		{
			_textureRect = static_cast<const BackendTexture2D*>(_currentPart->_texture.get())->full_rectangle();
		}

		void addRect(const RectF& position, const RectF& texture, const MarginsF& borders, const seir::Rgba32& color)
		{
			const auto textureSize = _currentPart->_texture->size();

			const auto px0 = position.left();
			const auto px1 = position.left() + borders._left * static_cast<float>(textureSize._width);
			const auto px2 = position.right() - borders._right * static_cast<float>(textureSize._width);
			const auto px3 = position.right();

			const auto py0 = position.top();
			const auto py1 = position.top() + borders._top * static_cast<float>(textureSize._height);
			const auto py2 = position.bottom() - borders._bottom * static_cast<float>(textureSize._height);
			const auto py3 = position.bottom();

			const bool has_left_border = px0 != px1;
			const bool has_right_border = px2 != px3;
			const bool has_top_border = py0 != py1;
			const bool has_bottom_border = py2 != py3;

			const auto row_vertices = 2 + static_cast<size_t>(has_left_border) + static_cast<size_t>(has_right_border);
			const auto stripe_count = 1 + static_cast<size_t>(has_top_border) + static_cast<size_t>(has_bottom_border);
			const auto vertex_count = row_vertices * (stripe_count + 1);
			const auto index_count = 2 * (stripe_count * row_vertices + stripe_count - 1);

			auto batch = prepareBatch(vertex_count, index_count);

			const auto tx0 = texture.left();
			const auto tx1 = texture.left() + borders._left;
			const auto tx2 = texture.right() - borders._right;
			const auto tx3 = texture.right();

			const auto ty0 = texture.top();
			const auto ty3 = texture.bottom();

			*batch._vertices++ = { { px0, py0 }, { tx0, ty0 }, color };
			if (has_left_border)
				*batch._vertices++ = { { px1, py0 }, { tx1, ty0 }, color };
			if (has_right_border)
				*batch._vertices++ = { { px2, py0 }, { tx2, ty0 }, color };
			*batch._vertices++ = { { px3, py0 }, { tx3, ty0 }, color };
			if (has_top_border)
			{
				const auto ty1 = texture.top() + borders._top;
				*batch._vertices++ = { { px0, py1 }, { tx0, ty1 }, color };
				if (has_left_border)
					*batch._vertices++ = { { px1, py1 }, { tx1, ty1 }, color };
				if (has_right_border)
					*batch._vertices++ = { { px2, py1 }, { tx2, ty1 }, color };
				*batch._vertices++ = { { px3, py1 }, { tx3, ty1 }, color };
			}
			if (has_bottom_border)
			{
				const auto ty2 = texture.bottom() - borders._bottom;
				*batch._vertices++ = { { px0, py2 }, { tx0, ty2 }, color };
				if (has_left_border)
					*batch._vertices++ = { { px1, py2 }, { tx1, ty2 }, color };
				if (has_right_border)
					*batch._vertices++ = { { px2, py2 }, { tx2, ty2 }, color };
				*batch._vertices++ = { { px3, py2 }, { tx3, ty2 }, color };
			}
			*batch._vertices++ = { { px0, py3 }, { tx0, ty3 }, color };
			if (has_left_border)
				*batch._vertices++ = { { px1, py3 }, { tx1, ty3 }, color };
			if (has_right_border)
				*batch._vertices++ = { { px2, py3 }, { tx2, ty3 }, color };
			*batch._vertices = { { px3, py3 }, { tx3, ty3 }, color };

			for (size_t i = 0; i < stripe_count; ++i)
			{
				if (i > 0)
				{
					*batch._indices++ = static_cast<uint16_t>(batch._baseIndex + row_vertices - 1);
					*batch._indices++ = static_cast<uint16_t>(batch._baseIndex);
				}
				for (size_t j = 0; j < row_vertices; ++j)
				{
					*batch._indices++ = static_cast<uint16_t>(batch._baseIndex + j);
					*batch._indices++ = static_cast<uint16_t>(batch._baseIndex + j + row_vertices);
				}
				batch._baseIndex += row_vertices;
			}
		}

		Batch prepareBatch(size_t vertexCount, size_t indexCount)
		{
			auto nextIndex = _currentPart->_vertices.size() / sizeof(Vertex2D);
			if (nextIndex > std::numeric_limits<uint16_t>::max() - vertexCount)
			{
				advancePart(_currentPart->_texture);
				nextIndex = 0;
			}
			const auto vertexBufferSize = _currentPart->_vertices.size() + sizeof(Vertex2D) * vertexCount;
			const auto indexBufferSize = _currentPart->_indices.size() + sizeof(uint16_t) * (indexCount + (nextIndex > 0 ? 2 : 0));
			_currentPart->_vertices.reserve(vertexBufferSize);
			_currentPart->_indices.reserve(indexBufferSize);
			auto* const vertices = reinterpret_cast<Vertex2D*>(_currentPart->_vertices.end());
			auto* indices = reinterpret_cast<uint16_t*>(_currentPart->_indices.end());
			_currentPart->_vertices.resize(vertexBufferSize);
			_currentPart->_indices.resize(indexBufferSize);
			if (nextIndex > 0)
			{
				*indices++ = static_cast<uint16_t>(nextIndex - 1);
				*indices++ = static_cast<uint16_t>(nextIndex);
			}
			return { vertices, indices, nextIndex };
		}

		void setTexture(const std::shared_ptr<const Texture2D>& texture)
		{
			assert(texture);
			if (_currentPart->_texture != texture)
			{
				if (_currentPart->_vertices.size() > 0)
					advancePart(texture);
				else
					_currentPart->_texture = texture;
			}
			_textureRect = _viewportData._renderer.map_rect({ { 0, 0 }, SizeF{ 1, 1 } }, static_cast<const BackendTexture2D*>(_currentPart->_texture.get())->orientation());
			_textureBorders = {};
		}

	private:
		void advancePart(const std::shared_ptr<const Texture2D>& texture)
		{
			if (_currentPart != &_parts.back())
			{
				++_currentPart;
				_currentPart->_texture = texture;
			}
			else
				_currentPart = &_parts.emplace_back(texture);
		}
	};

	Renderer2D::Renderer2D(Viewport& viewport)
		: _data{ std::make_unique<Renderer2DData>(*viewport._data) }
	{
	}

	Renderer2D::~Renderer2D() noexcept = default;

	void Renderer2D::addQuad(const Quad& quad)
	{
		const auto batch = _data->prepareBatch(4, 4);

		batch._vertices[0] = { quad._a, _data->_textureRect.topLeft(), _data->_color };
		batch._vertices[1] = { quad._d, _data->_textureRect.bottomLeft(), _data->_color };
		batch._vertices[2] = { quad._b, _data->_textureRect.topRight(), _data->_color };
		// cppcheck-suppress unreadVariable
		batch._vertices[3] = { quad._c, _data->_textureRect.bottomRight(), _data->_color };

		batch._indices[0] = static_cast<uint16_t>(batch._baseIndex);
		batch._indices[1] = static_cast<uint16_t>(batch._baseIndex + 1);
		batch._indices[2] = static_cast<uint16_t>(batch._baseIndex + 2);
		// cppcheck-suppress unreadVariable
		batch._indices[3] = static_cast<uint16_t>(batch._baseIndex + 3);
	}

	size_t Renderer2D::addBorderlessRect(const RectF& rect)
	{
		auto batch = _data->prepareBatch(4, 4);

		batch._vertices[0] = { rect.topLeft(), _data->_textureRect.topLeft(), _data->_color };
		batch._vertices[1] = { rect.bottomLeft(), _data->_textureRect.bottomLeft(), _data->_color };
		batch._vertices[2] = { rect.topRight(), _data->_textureRect.topRight(), _data->_color };
		// cppcheck-suppress unreadVariable
		batch._vertices[3] = { rect.bottomRight(), _data->_textureRect.bottomRight(), _data->_color };

		batch._indices[0] = static_cast<uint16_t>(batch._baseIndex);
		batch._indices[1] = static_cast<uint16_t>(batch._baseIndex + 1);
		batch._indices[2] = static_cast<uint16_t>(batch._baseIndex + 2);
		// cppcheck-suppress unreadVariable
		batch._indices[3] = static_cast<uint16_t>(batch._baseIndex + 3);

		return (static_cast<size_t>(_data->_currentPart - _data->_parts.data()) << 16) + batch._baseIndex;
	}

	void Renderer2D::addRect(const RectF& rect)
	{
		_data->addRect(rect, _data->_textureRect, _data->_textureBorders, _data->_color);
	}

	void Renderer2D::draw(RenderPass& pass)
	{
		PushProgram program{ pass, _data->_viewportData._renderer_builtin._program_2d.get() };
		const auto viewport_size = pass.viewport_rect().size();
		_data->_viewportData._renderer_builtin._program_2d->set_uniform("mvp", seir::Mat4::projection2D(viewport_size._width, viewport_size._height));
		for (auto& part : _data->_parts)
		{
			if (part._vertices.size() > 0)
			{
				assert(part._indices.size() > 0);
				assert(part._vertices.size() / sizeof(Vertex2D) <= size_t{ std::numeric_limits<uint16_t>::max() } + 1);
				{
					PushTexture texture{ pass, part._texture.get(), Texture2D::TrilinearFilter };
					static_cast<RenderPassImpl&>(pass).flush_2d(part._vertices, part._indices);
				}
				part._vertices.clear();
				part._indices.clear();
			}
			else
				assert(part._indices.size() == 0);
			part._texture = _data->_viewportData._renderer_builtin._white_texture;
		}
		_data->_currentPart = &_data->_parts.front();
		_data->_color = seir::Rgba32::white();
		_data->_textureRect = static_cast<const BackendTexture2D*>(_data->_currentPart->_texture.get())->full_rectangle();
		_data->_textureBorders = {};
	}

	void Renderer2D::rewriteBorderlessRect(size_t id, const RectF& rect)
	{
		const auto partIndex = id >> 16;
		assert(partIndex <= static_cast<size_t>(_data->_currentPart - _data->_parts.data()));
		auto& vertexBuffer = _data->_parts[partIndex]._vertices;
		const auto vertexIndex = id & 0xffff;
		assert(vertexIndex + 4 <= vertexBuffer.size() / sizeof(Vertex2D));
		auto* const vertices = static_cast<Vertex2D*>(vertexBuffer.data()) + vertexIndex;
		vertices[0]._position = rect.topLeft();
		vertices[1]._position = rect.bottomLeft();
		vertices[2]._position = rect.topRight();
		vertices[3]._position = rect.bottomRight();
	}

	void Renderer2D::setColor(const seir::Rgba32& color)
	{
		_data->_color = color;
	}

	void Renderer2D::setTexture(const std::shared_ptr<const Texture2D>& texture)
	{
		_data->setTexture(texture ? texture : _data->_viewportData._renderer_builtin._white_texture);
	}

	void Renderer2D::setTextureRect(const RectF& rect, const MarginsF& borders)
	{
		const auto minimumSize = SizeF{ borders._left + 1 + borders._right, borders._top + 1 + borders._bottom };
		if (rect.width() >= minimumSize._width && rect.height() >= minimumSize._height)
		{
			const SizeF textureSize{ _data->_currentPart->_texture->size() };
			_data->_textureRect = _data->_viewportData._renderer.map_rect(rect / seir::Vec2{ textureSize._width, textureSize._height }, static_cast<const BackendTexture2D*>(_data->_currentPart->_texture.get())->orientation());
			_data->_textureBorders = {
				borders._top / textureSize._height,
				borders._right / textureSize._width,
				borders._bottom / textureSize._height,
				borders._left / textureSize._width,
			};
		}
	}

	SizeF Renderer2D::viewportSize() const noexcept
	{
		return SizeF{ _data->_viewportData._window_size };
	}
}
