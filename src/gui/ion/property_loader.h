#ifndef __GUI_ION_PROPERTY_LOADER_H
#define __GUI_ION_PROPERTY_LOADER_H

#include <yttrium/renderer/texture.h>

#include "../property_loader.h"

namespace Yttrium
{

namespace Ion
{

class Node;
class Object;

} // namespace Ion

class GuiImpl;

class GuiIonPropertyLoader: public GuiPropertyLoader
{
public:

	GuiIonPropertyLoader(const Ion::Object *object, const Ion::Object *class_, GuiImpl *gui);

public: // GuiPropertyLoader

	void bind(const StaticString &name) override;

	bool load_alignment(const StaticString &name, Alignment *alignment) const override;

	bool load_color(const StaticString &name, Vector4f *color) const override;

	bool load_font(const StaticString &name, TextureFont *font, Texture2DPtr *texture) const override;

	bool load_margins(const StaticString &name, MarginsI *margins) const override;

	bool load_position(const StaticString &name, Vector2f *position) const override;

	bool load_rect(const StaticString &name, Rect *rect, bool update) const override;

	bool load_scaling(const StaticString &name, Scaling *scaling) const override;

	bool load_size(const StaticString &name, Vector2f *size) const override;

	SoundPtr load_sound(const StaticString &name) const override;

	bool load_state(const StaticString &name, WidgetState *state) const override;

	bool load_text(const StaticString &name, String *text) const override;

	bool load_texture(const StaticString &name, Texture2DPtr *texture) const override;

	void unbind() override;

public:

	static bool load_alignment(Alignment *alignment, const Ion::Node &node);

	static bool load_margins(MarginsI *margins, const Ion::Node &node);

	static bool load_scaling(Scaling *scaling, const Ion::Node &node);

	static bool load_size(Vector2f *size, const Ion::Node &node);

	static SoundPtr load_sound(const Ion::Node &node);

	static bool load_state(WidgetState *state, const Ion::Node &node);

	static bool load_text(const StaticString **text, const Ion::Object &object, const StaticString &name);

	static bool load_texture(Texture2DPtr *texture, const Ion::Node &node,
		TextureCache *texture_cache, Texture2D::Filter default_filter);

private:

	const Ion::Object *_object;
	const Ion::Object *_class;
	const GuiImpl     *_gui;
	TextureCache      *_texture_cache;
	const Ion::Object *_bound_object;
	const Ion::Object *_bound_class;
};

} // namespace Yttrium

#endif // __GUI_ION_PROPERTY_LOADER_H
