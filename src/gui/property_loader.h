#ifndef __GUI_PROPERTY_LOADER_H
#define __GUI_PROPERTY_LOADER_H

#include <yttrium/audio/sound.h>

#include "property.h"
#include "types.h"

namespace Yttrium
{

class TextureFont;

namespace Gui
{

class PropertyLoader
{
public:

	virtual ~PropertyLoader() {}

public:

	// NOTE: The loading functions MAY modify the data even if they haven't finished successfully.

	virtual void bind(const StaticString &name) = 0;

	virtual bool load_alignment(const StaticString &name, Alignment *alignment) = 0;

	virtual bool load_color(const StaticString &name, Vector4f *color) = 0;

	virtual bool load_font(const StaticString &name, TextureFont *font, Texture2DPtr *texture) = 0;

	virtual bool load_position(const StaticString &name, Vector3f *color) = 0;

	virtual bool load_scaling(const StaticString &name, Scaling *scaling) = 0;

	virtual bool load_size(const StaticString &name, Vector2f *size) = 0;

	virtual SoundPtr load_sound(const StaticString &name) = 0;

	virtual bool load_state(const StaticString &name, WidgetState *state) = 0;

	virtual bool load_text(const StaticString &name, String *text) = 0;

	virtual bool load_texture(const StaticString &name, TextureProperty *texture) = 0;

	virtual void unbind() = 0;
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_PROPERTY_LOADER_H
