#ifndef __GUI_LOADER_H
#define __GUI_LOADER_H

#include <yttrium/audio/sound.h>
#include <yttrium/renderer/pointers.h>

#include "types.h"

namespace Yttrium
{

class TextureFont;

class GuiPropertyLoader
{
public:

	virtual ~GuiPropertyLoader() {}

public:

	// NOTE: The loading functions MAY modify the data even if they haven't finished successfully.

	virtual void bind(const StaticString &name) = 0;

	virtual bool load_alignment(const StaticString &name, Alignment *alignment) const = 0;

	virtual bool load_color(const StaticString &name, Vector4f *color) const = 0;

	virtual bool load_font(const StaticString &name, TextureFont *font, Texture2DPtr *texture) const = 0;

	virtual bool load_margins(const StaticString &name, MarginsI *margins) const = 0;

	virtual bool load_position(const StaticString &name, Vector2f *position) const = 0;

	virtual bool load_rect(const StaticString &name, RectI *rect, bool update = false) const = 0;

	virtual bool load_scaling(const StaticString &name, Scaling *scaling) const = 0;

	virtual bool load_size(const StaticString &name, Vector2f *size) const = 0;

	virtual SoundPtr load_sound(const StaticString &name) const = 0;

	virtual bool load_state(const StaticString &name, WidgetState *state) const = 0;

	virtual bool load_text(const StaticString &name, String *text) const = 0;

	virtual bool load_texture(const StaticString &name, Texture2DPtr *texture) const = 0;

	virtual void unbind() = 0;
};

} // namespace Yttrium

#endif // __GUI_LOADER_H
