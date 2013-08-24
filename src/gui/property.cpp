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
		dumper->dump_margins("texture_borders", borders);
	}
}

bool TextureProperty::load(PropertyLoader &loader)
{
	if (!loader.load_texture("texture", &texture) && texture.is_null())
		return false;

	if (!loader.load_rect("texture_rect", &rect) && rect.is_empty())
		rect = texture->rect();

	loader.load_margins("texture_borders", &borders);

	return true;
}

} // namespace Gui

} // namespace Yttrium
