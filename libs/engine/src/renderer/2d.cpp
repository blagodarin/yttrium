// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/renderer/2d.h>

#include <yttrium/math/margins.h>
#include <yttrium/math/matrix.h>
#include <yttrium/math/quad.h>
#include <yttrium/math/rect.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/program.h>
#include <yttrium/renderer/viewport.h>
#include "2d.h"
#include "texture.h"
#include "viewport.h"

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

			Part(const std::shared_ptr<const Texture2D>& texture) noexcept
				: _texture{ texture } {}
		};

		const ViewportData& _viewportData;
		std::vector<Part> _parts;
		Part* _currentPart = nullptr;
		Bgra32 _color = Bgra32::white();
		RectF _textureRect;
		MarginsF _textureBorders;

		struct Batch
		{
			Vertex2D* _vertices = nullptr;
			uint16_t* _indices = nullptr;
			size_t _baseIndex = 0;
		};

		Renderer2DData(const ViewportData& viewportData)
			: _viewportData{ viewportData }
			, _currentPart{ &_parts.emplace_back(_viewportData._renderer_builtin._white_texture) }
		{
			_textureRect = static_cast<const BackendTexture2D*>(_currentPart->_texture.get())->full_rectangle();
		}

		void addRect(const RectF& position, const RectF& texture, Bgra32 color)
		{
			auto batch = prepareBatch(4, 4);

			batch._vertices[0] = { position.top_left(), texture.top_left(), color };
			batch._vertices[1] = { position.bottom_left(), texture.bottom_left(), color };
			batch._vertices[2] = { position.top_right(), texture.top_right(), color };
			batch._vertices[3] = { position.bottom_right(), texture.bottom_right(), color };

			batch._indices[0] = static_cast<uint16_t>(batch._baseIndex);
			batch._indices[1] = static_cast<uint16_t>(batch._baseIndex + 1);
			batch._indices[2] = static_cast<uint16_t>(batch._baseIndex + 2);
			batch._indices[3] = static_cast<uint16_t>(batch._baseIndex + 3);
		}

		void addRect(const RectF& position, const RectF& texture, const MarginsF& borders, Bgra32 color)
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
				if (_currentPart->_vertices.size() > 0)
					advancePart(texture);
				else
					_currentPart->_texture = texture;
			_textureRect = static_cast<const BackendTexture2D*>(texture.get())->full_rectangle();
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

		batch._vertices[0] = { quad._a, _data->_textureRect.top_left(), _data->_color };
		batch._vertices[1] = { quad._d, _data->_textureRect.bottom_left(), _data->_color };
		batch._vertices[2] = { quad._b, _data->_textureRect.top_right(), _data->_color };
		batch._vertices[3] = { quad._c, _data->_textureRect.bottom_right(), _data->_color };

		batch._indices[0] = static_cast<uint16_t>(batch._baseIndex);
		batch._indices[1] = static_cast<uint16_t>(batch._baseIndex + 1);
		batch._indices[2] = static_cast<uint16_t>(batch._baseIndex + 2);
		batch._indices[3] = static_cast<uint16_t>(batch._baseIndex + 3);
	}

	void Renderer2D::addRect(const RectF& rect)
	{
		_data->addRect(rect, _data->_textureRect, _data->_textureBorders, _data->_color);
	}

	void Renderer2D::draw(RenderPass& pass)
	{
		PushProgram program{ pass, _data->_viewportData._renderer_builtin._program_2d.get() };
		_data->_viewportData._renderer_builtin._program_2d->set_uniform("mvp", Matrix4::projection_2d(pass.viewport_rect().size()));
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
		_data->_color = Bgra32::white();
		_data->_textureRect = static_cast<const BackendTexture2D*>(_data->_currentPart->_texture.get())->full_rectangle();
		_data->_textureBorders = {};
	}

	void Renderer2D::setColor(Bgra32 color)
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
			_data->_textureRect = _data->_viewportData._renderer.map_rect(rect / Vector2{ textureSize._width, textureSize._height }, static_cast<const BackendTexture2D*>(_data->_currentPart->_texture.get())->orientation());
			_data->_textureBorders = {
				borders._top / textureSize._height,
				borders._right / textureSize._width,
				borders._bottom / textureSize._height,
				borders._left / textureSize._width,
			};
		}
	}
}
