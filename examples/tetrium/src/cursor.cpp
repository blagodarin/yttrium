#include "cursor.h"

#include <yttrium/index_buffer.h>
#include <yttrium/math/vector.h>
#include <yttrium/renderer.h>
#include <yttrium/renderer_modifiers.h>
#include <yttrium/timer.h>
#include <yttrium/vertex_buffer.h>

#include <array>

struct Vertex // TODO: Don't rely on any specific GUI vertex format.
{
	Vector2 position;
	Vector4 color;
	Vector2 texture;

	Vertex(const Vector2& position, const Vector4& color)
		: position(position)
		, color(color)
		, texture(0, 0)
	{
	}
};

const std::array<uint16_t, 36> indices = {0, 1, 2};

Cursor::Cursor(Renderer& renderer)
	: _renderer(renderer)
	, _vertices(_renderer.create_vertex_buffer({VA::f2, VA::f4, VA::f2}, 3 * sizeof(Vertex)))
	, _indices(_renderer.create_index_buffer(IndexFormat::U16, indices.size(), indices.data()))
{
}

Cursor::~Cursor() = default;

void Cursor::draw(const PointF& point)
{
	const auto animation = [](Clock tick)
	{
		constexpr Clock period = 1500;
		constexpr float half_period = period / 2;

		const auto step = tick % period;
		return (step < half_period ? half_period - step : step - half_period) / half_period;
	};

	const auto tick = Timer::clock();

	const auto a = animation(tick);
	const auto b = animation(tick + 500);
	const auto c = animation(tick + 1000);

	const std::array<Vertex, 3> vertices =
	{
		Vertex({point.x(),      point.y()     }, {1, a, a}),
		Vertex({point.x() + 24, point.y() + 48}, {b, 1, b}),
		Vertex({point.x() + 48, point.y() + 24}, {c, c, 1}),
	};
	_vertices->write(0, vertices.size(), vertices.data());
	PushTexture push_texture(_renderer, nullptr);
	_renderer.draw_triangles(*_vertices, *_indices);
}
