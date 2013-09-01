#include "property.h"

#include <yttrium/renderer/texture.h>

#include "property_dumper.h"
#include "property_loader.h"

namespace Yttrium
{

namespace Gui
{

void TextureProperty::dump(PropertyDumper *dumper) const
{
	if (!texture.is_null())
	{
		dumper->dump_texture("texture", texture);
		dumper->dump_rect("texture_rect", rect);
		dumper->dump_margins("borders", borders);
	}
}

bool TextureProperty::load(const PropertyLoader &loader)
{
	if (!loader.load_texture("texture", &texture))
		return false;

	if (!loader.load_rect("texture_rect", &rect))
		rect = texture->rect();

	loader.load_margins("borders", &borders);

	return true;
}

void TextureProperty::update(const PropertyLoader &loader)
{
	loader.load_texture("texture", &texture);
	loader.load_rect("texture_rect", &rect, true);
	loader.load_margins("borders", &borders);
}

} // namespace Gui

} // namespace Yttrium
