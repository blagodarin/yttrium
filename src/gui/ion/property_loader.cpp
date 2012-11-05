#include "../logging.h"

#include "property_loader.h"

#include <Yttrium/ion/object.h>
#include <Yttrium/ion/value.h>
#include <Yttrium/renderer/texture_cache.h>
#include <Yttrium/utils.h>

#include "../manager.h"

namespace Yttrium
{

namespace Gui
{

namespace
{

unsigned read_color(Vector4f *color, const Ion::Node &node, unsigned inherit)
{
	unsigned result = 0;

	Ion::Node::ConstRange values = node.values();

	if (!values.is_empty())
	{
		size_t items = min<size_t>(values.size(), 4);

		for (size_t i = 0; i < items; values.pop_first(), ++i)
		{
			const StaticString *value;

			if (values.first().get(&value) && value->to_number(color->data() + i))
			{
				result |= 1 << i;
			}
		}

		if (items < 4 && !(inherit & (1 << 3)))
		{
			color->a = 1.f;
			result |= 1 << 3;
		}
	}

	return result;
}

unsigned read_position(Vector3f *position, const Ion::Node &node, unsigned inherit)
{
	unsigned result = 0;

	Ion::Node::ConstRange values = node.values();

	if (!values.is_empty())
	{
		size_t items = min<size_t>(values.size(), 3);

		for (size_t i = 0; i < items; values.pop_first(), ++i)
		{
			const StaticString *value;

			if (values.first().get(&value) && value->to_number(position->data() + i))
			{
				result |= 1 << i;
			}
		}

		if (items < 3 && !(inherit & (1 << 2)))
		{
			position->z = 1.f;
			result |= 1 << 2;
		}
	}

	return result;
}

unsigned read_size(Vector2f *size, const Ion::Node &node, unsigned inherit)
{
	unsigned result = 0;

	Ion::Node::ConstRange values = node.values();

	if (!values.is_empty())
	{
		size_t items = min<size_t>(values.size(), 2);

		for (size_t i = 0; i < items; values.pop_first(), ++i)
		{
			const StaticString *value;

			if (values.first().get(&value) && value->to_number(size->data() + i))
			{
				result |= 1 << i;
			}
		}

		if (items < 2 && !(inherit & (1 << 1)))
		{
			size->y = 1.f;
			result |= 1 << 1;
		}
	}

	return result;
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IonPropertyLoader::IonPropertyLoader(const Ion::Object *object, const Ion::Object *class_, ManagerImpl *manager)
	: _object(object)
	, _class(class_)
	, _manager(manager)
	, _texture_cache(manager->renderer().texture_cache())
{
}

bool IonPropertyLoader::load_alignment(const StaticString &name, Alignment *alignment)
{
	Y_LOG_TRACE("[Gui.Loader] Loading alignment...");

	const Ion::Node *node = _object->last(name);

	if (node && load_alignment(alignment, *node))
	{
		return true;
	}

	if (_class)
	{
		node = _class->last(name);

		if (node && load_alignment(alignment, *node))
		{
			return true;
		}
	}

	return false;
}

bool IonPropertyLoader::load_color(const StaticString &name, Vector4f *color)
{
	Y_LOG_TRACE("[Gui.Loader] Loading color...");

	unsigned loaded = 0;

	const Ion::Node *node;

	if (_class)
	{
		node = _class->last(name);
		if (node)
		{
			loaded = read_color(color, *node, 0);
		}
	}

	node = _object->last(name);
	if (node)
	{
		loaded |= read_color(color, *node, loaded);
	}

	return loaded == 15;
}

bool IonPropertyLoader::load_font(const StaticString &name, TextureFont *font, Texture2D *texture)
{
	Y_LOG_TRACE("[Gui.Loader] Loading font...");

	const StaticString *font_name;

	const Ion::Node *node = _object->last(name);

	if (!node || node->size() != 1 || !node->first()->get(&font_name))
	{
		if (!_class)
		{
			return false;
		}

		node = _class->last(name);

		if (!node || node->size() != 1 || !node->first()->get(&font_name))
		{
			return false;
		}
	}

	const ManagerImpl::FontDesc *font_desc = _manager->font(*font_name);

	if (!font_desc)
	{
		return false;
	}

	*font = font_desc->font;
	*texture = font_desc->texture;
	return true;
}

bool IonPropertyLoader::load_position(const StaticString &name, Vector3f *position)
{
	Y_LOG_TRACE("[Gui.Loader] Loading position...");

	unsigned loaded = 0;

	const Ion::Node *node;

	if (_class)
	{
		node = _class->last(name);
		if (node)
		{
			loaded = read_position(position, *node, 0);
		}
	}

	node = _object->last(name);
	if (node)
	{
		loaded |= read_position(position, *node, loaded);
	}

	return loaded == 7;
}

bool IonPropertyLoader::load_scaling(const StaticString &name, Scaling *scaling)
{
	Y_LOG_TRACE("[Gui.Loader] Loading scaling...");

	const Ion::Node *node = _object->last(name);

	if (node && load_scaling(scaling, *node))
	{
		return true;
	}

	if (_class)
	{
		node = _class->last(name);

		if (node && load_scaling(scaling, *node))
		{
			return true;
		}
	}

	return false;
}

bool IonPropertyLoader::load_size(const StaticString &name, Vector2f *size)
{
	Y_LOG_TRACE("[Gui.Loader] Loading size...");

	unsigned loaded = 0;

	const Ion::Node *node;

	if (_class)
	{
		node = _class->last(name);
		if (node)
		{
			loaded = read_size(size, *node, 0);
		}
	}

	node = _object->last(name);
	if (node)
	{
		loaded |= read_size(size, *node, loaded);
	}

	return loaded == 3;
}

bool IonPropertyLoader::load_text(const StaticString &name, String *text)
{
	Y_LOG_TRACE("[Gui.Loader] Loading text...");

	const StaticString *value;

	if (!load_text(&value, *_object, name))
	{
		return false;
	}

	*text = *value;
	return true;
}

bool IonPropertyLoader::load_texture(const StaticString &name, Texture2D *texture)
{
	Y_LOG_TRACE("[Gui.Loader] Loading texture...");

	const Ion::Node *node = _object->last(name);

	if (node && load_texture(texture, *node, &_texture_cache,
		Texture2D::TrilinearFilter | Texture2D::AnisotropicFilter))
	{
		return true;
	}

	if (_class)
	{
		node = _class->last(name);

		if (node && load_texture(texture, *node, &_texture_cache,
			Texture2D::TrilinearFilter | Texture2D::AnisotropicFilter))
		{
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool IonPropertyLoader::load_alignment(Alignment *alignment, const Ion::Node &node)
{
	// TODO: Inheritance.

	Ion::Node::ConstRange values = node.values();

	if (values.is_empty() || values.size() > 2)
	{
		return false;
	}

	Alignment result = CenterAlignment;
	bool is_centered = false;

	for (; !values.is_empty(); values.pop_first())
	{
		const StaticString *value;

		if (!values.first().get(&value))
		{
			return false;
		}
		else if (*value == S("center"))
		{
			if (is_centered)
			{
				return false;
			}
			is_centered = true;
		}
		else if (*value == S("left"))
		{
			if (result & HorizontalAlignmentMask)
			{
				return false;
			}
			result |= LeftAlignment;
		}
		else if (*value == S("right"))
		{
			if (result & HorizontalAlignmentMask)
			{
				return false;
			}
			result |= RightAlignment;
		}
		else if (*value == S("top"))
		{
			if (result & VerticalAlignmentMask)
			{
				return false;
			}
			result |= TopAlignment;
		}
		else if (*value == S("bottom"))
		{
			if (result & VerticalAlignmentMask)
			{
				return false;
			}
			result |= BottomAlignment;
		}
		else
		{
			return false;
		}
	}

	*alignment = result;
	return true;
}

bool IonPropertyLoader::load_scaling(Scaling *scaling, const Ion::Node &node)
{
	Ion::Node::ConstRange values = node.values();

	if (values.size() != 1)
	{
		return false;
	}

	const StaticString *value;

	if (!values.first().get(&value))
	{
		return false;
	}
	else if (*value == S("fit"))
	{
		*scaling = Scaling::Fit;
	}
	else if (*value == S("max"))
	{
		*scaling = Scaling::Max;
	}
	else if (*value == S("min"))
	{
		*scaling = Scaling::Min;
	}
	else if (*value == S("stretch"))
	{
		*scaling = Scaling::Stretch;
	}
	else
	{
		return false;
	}

	return true;
}

bool IonPropertyLoader::load_size(Vector2f *size, const Ion::Node &node)
{
	return read_size(size, node, 0) == 3;
}

bool IonPropertyLoader::load_text(const StaticString **text, const Ion::Object &object, const StaticString &name)
{
	const Ion::Node *node = object.last(name);

	return node && node->size() == 1 && node->first()->get(text);
}

bool IonPropertyLoader::load_texture(Texture2D *texture, const Ion::Node &node,
	TextureCache *texture_cache, Texture2D::Filter default_filter)
{
	Ion::Node::ConstRange values = node.values();

	if (values.is_empty() || values.size() > 3)
	{
		return false;
	}

	const StaticString *texture_name;

	if (!values.first().get(&texture_name))
	{
		return false;
	}

	values.pop_first();

	Texture2D::Filter filter = default_filter;

	if (!values.is_empty())
	{
		bool has_interpolation = false;
		bool has_anisotropy = false;

		for (; !values.is_empty(); values.pop_first())
		{
			const StaticString *value;

			if (!values.first().get(&value))
			{
				return false;
			}
			else if (*value == S("nearest"))
			{
				if (has_interpolation)
				{
					return false;
				}
				filter = Texture2D::NearestFilter | (filter & Texture2D::AnisotropicFilter);
				has_interpolation = true;
			}
			else if (*value == S("linear"))
			{
				if (has_interpolation)
				{
					return false;
				}
				filter = Texture2D::LinearFilter | (filter & Texture2D::AnisotropicFilter);
				has_interpolation = true;
			}
			else if (*value == S("bilinear"))
			{
				if (has_interpolation)
				{
					return false;
				}
				filter = Texture2D::BilinearFilter | (filter & Texture2D::AnisotropicFilter);
				has_interpolation = true;
			}
			else if (*value == S("trilinear"))
			{
				if (has_interpolation)
				{
					return false;
				}
				filter = Texture2D::TrilinearFilter | (filter & Texture2D::AnisotropicFilter);
				has_interpolation = true;
			}
			else if (*value == S("anisotropic"))
			{
				if (has_anisotropy)
				{
					return false;
				}
				filter = (filter & Texture2D::IsotropicFilterMask) | Texture2D::AnisotropicFilter;
				has_anisotropy = true;
			}
			else
			{
				return false;
			}
		}
	}

	Texture2D result_texture = texture_cache->load_texture_2d(*texture_name);

	if (!result_texture)
	{
		return false;
	}

	*texture = result_texture;
	texture->set_filter(filter);
	return true;
}

} // namespace Gui

} // namespace Yttrium
