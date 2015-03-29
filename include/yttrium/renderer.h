/// \file
/// \brief

#ifndef __Y_RENDERER_H
#define __Y_RENDERER_H

#include <yttrium/rect.h>
#include <yttrium/renderer/pointers.h>
#include <yttrium/static_string.h>
#include <yttrium/texture_font.h>

#include <memory> // unique_ptr

namespace Yttrium
{

class TextureCache;
class WindowBackend;

///
class Y_API Renderer
{
	friend TextureCache;
	friend WindowBackend;

public:

	struct TextCapture
	{
		const unsigned cursor_pos;
		bool           has_cursor;
		RectF          cursor_rect;

		const unsigned selection_begin;
		const unsigned selection_end;
		bool           has_selection;
		RectF          selection_rect;

		TextCapture(unsigned cursor_pos, unsigned selection_begin, unsigned selection_size)
			: cursor_pos(cursor_pos)
			, has_cursor(false)
			, selection_begin(selection_begin)
			, selection_end(selection_begin + selection_size)
			, has_selection(false)
		{
		}
	};

public:

	///
	~Renderer();

	///
	Allocator* allocator() const;

	///
	void begin_frame(); // TODO: Remove.

	///
	/// \note Texture cache lifetime must not exceed its renderer lifetime.
	std::unique_ptr<TextureCache> create_texture_cache();

	///
	void draw_rectangle(const RectF& rect);

	///
	void draw_rectangle(float x, float y, float width, float height)
	{
		draw_rectangle(RectF(x, y, width, height));
	}

	///
	void draw_rectangle(const RectF& rect, const RectF& texture_rect);

	///
	void draw_text(const Vector2f& position, const StaticString& text,
		unsigned alignment = BottomRightAlignment, TextCapture* capture = nullptr);

	///
	void end_frame(); // TODO: Remove.

	///
	void flush_2d();

	///
	Vector2d rendering_size() const;

	///
	void set_color(const Vector4f& color);

	///
	bool set_font(const TextureFont& font);

	///
	void set_font_size(const Vector2f& size);

	///
	void set_font_size(float y_size, float x_scaling = 1)
	{
		set_font_size(Vector2f(x_scaling, y_size));
	}

	///
	void set_matrix_2d(double width, double height);

	///
	void set_matrix_2d_height(double height);

	///
	void set_matrix_2d_width(double width);

	///
	void set_texture(const Texture2DPtr& texture);

	///
	bool set_texture_borders(const MarginsI& borders);

	///
	void set_texture_rectangle(const RectF& rect);

	///
	void set_texture_rectangle(float x, float y, float width, float height)
	{
		set_texture_rectangle(RectF(x, y, width, height));
	}

	/// Take a screenshot.
	/// \param name
	/// \note The screenshot would be actually taken at the end of the frame
	/// and saved in the PNG format.
	void take_screenshot(const StaticString& name);

	///
	Vector2f text_size(const StaticString& text) const;

	///
	Dim2 viewport_size() const;

	Renderer() = delete;
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer&&) = delete;

public:

	class Private;

private:

	Private* _private;

private:

	Y_PRIVATE Renderer(WindowBackend& window, Allocator* allocator);
	Y_PRIVATE Renderer(Private* private_);
};

} // namespace Yttrium

#endif // __Y_RENDERER_H
