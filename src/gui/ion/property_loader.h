#ifndef __GUI_ION_PROPERTY_LOADER_H
#define __GUI_ION_PROPERTY_LOADER_H

#include <Yttrium/ion/node.h>
#include <Yttrium/renderer/texture_cache.h>

#include "../property_loader.h"

namespace Yttrium
{

namespace Gui
{

class IonPropertyLoader: public PropertyLoader
{
public:

	IonPropertyLoader(const Ion::Object *object, const Ion::Object *class_, const TextureCache &texture_cache);

public: // PropertyLoader

	virtual bool load_alignment(const StaticString &name, Alignment *alignment);

	virtual bool load_color(const StaticString &name, Vector4f *color);

	virtual bool load_position(const StaticString &name, Vector3f *color);

	virtual bool load_scaling(const StaticString &name, Scaling *scaling);

	virtual bool load_size(const StaticString &name, Vector2f *size);

	virtual bool load_texture(const StaticString &name, Texture2D *texture);

public:

	static bool load_alignment(Alignment *alignment, const Ion::Node &node);

	static bool load_scaling(Scaling *scaling, const Ion::Node &node);

	static bool load_size(Vector2f *size, const Ion::Node &node);

	static bool load_text(const StaticString **text, const Ion::Object &object, const StaticString &name);

	static bool load_texture(Texture2D *texture, const Ion::Node &node,
		TextureCache *texture_cache, Texture2D::Filter default_filter);

private:

	const Ion::Object *_object;
	const Ion::Object *_class;
	TextureCache       _texture_cache;
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_ION_PROPERTY_LOADER_H
