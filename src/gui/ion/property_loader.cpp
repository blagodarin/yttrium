#include "../logging.h"

#include "property_loader.h"

#include <yttrium/audio/sound.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/margins.h>
#include <yttrium/renderer/texture_cache.h>
#include <yttrium/utils.h>

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

			if (values->get(&value) && value->to_number(color->data() + i))
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

			if (values->get(&value) && value->to_number(position->data() + i))
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

void read_rect(const Ion::Node &node, Integer elements[4])
{
	Ion::Node::ConstRange values = node.values();

	for (Integer i = 0; i < 4; ++i)
	{
		if (values.is_empty())
			break;

		const StaticString *value;
		if (Y_LIKELY(values->get(&value)))
		{
			Integer number;
			if (value->to_number(&number) && Y_LIKELY(number >= i >> 1)) // 'number' must be not less than {0, 0, 1, 1}.
				elements[i] = number;
		}
		
		values.pop_first();
	}
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

			if (values->get(&value) && value->to_number(size->data() + i))
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
	, _texture_cache(manager->texture_cache())
	, _bound_object(_object)
	, _bound_class(_class)
{
}

void IonPropertyLoader::bind(const StaticString &name)
{
	_bound_object = nullptr;

	const Ion::Node *node = &_object->last(name);
	if (node->exists() && node->size() != 1)
		node->first()->get(&_bound_object);

	_bound_class = nullptr;

	if (_class)
	{
		const Ion::Node *node = &_class->last(name);
		if (node->size() == 1)
			node->first()->get(&_bound_class);
	}
}

bool IonPropertyLoader::load_alignment(const StaticString &name, Alignment *alignment) const
{
	Y_LOG_TRACE("[Gui.Loader] Loading alignment...");

	if (_bound_object)
	{
		const Ion::Node *node = &_bound_object->last(name);
		if (node->exists() && load_alignment(alignment, *node))
			return true;
	}

	if (_bound_class)
	{
		const Ion::Node *node = &_bound_class->last(name);
		if (node->exists() && load_alignment(alignment, *node))
			return true;
	}

	return false;
}

bool IonPropertyLoader::load_color(const StaticString &name, Vector4f *color) const
{
	Y_LOG_TRACE("[Gui.Loader] Loading color...");

	unsigned loaded = 0x0;

	if (_bound_class)
	{
		const Ion::Node *node = &_bound_class->last(name);
		if (node->exists())
			loaded = read_color(color, *node, 0x0);
	}

	if (_bound_object)
	{
		const Ion::Node *node = &_bound_object->last(name);
		if (node->exists())
			loaded |= read_color(color, *node, loaded);
	}

	return loaded == 0xF;
}

bool IonPropertyLoader::load_font(const StaticString &name, TextureFont *font, Texture2DPtr *texture) const
{
	Y_LOG_TRACE("[Gui.Loader] Loading font...");

	const StaticString *font_name;

	const Ion::Node *node = _bound_object ? &_bound_object->last(name) : nullptr;

	if (!node->exists() || node->size() != 1 || !node->first()->get(&font_name))
	{
		if (!_bound_class)
			return false;

		node = &_bound_class->last(name);

		if (!node->exists() || node->size() != 1 || !node->first()->get(&font_name))
			return false;
	}

	const ManagerImpl::FontDesc *font_desc = _manager->font(*font_name);

	if (!font_desc)
		return false;

	*font = font_desc->font;
	*texture = font_desc->texture;

	return true;
}

bool IonPropertyLoader::load_margins(const StaticString &name, MarginsI *margins) const
{
	Y_LOG_TRACE("[Gui.Loader] Loading margins...");

	if (_bound_object)
	{
		const Ion::Node *node = &_bound_object->last(name);
		if (node->exists() && load_margins(margins, *node))
			return true;
	}

	if (_bound_class)
	{
		const Ion::Node *node = &_bound_class->last(name);
		if (node->exists() && load_margins(margins, *node))
			return true;
	}

	return false;
}

bool IonPropertyLoader::load_position(const StaticString &name, Vector3f *position) const
{
	Y_LOG_TRACE("[Gui.Loader] Loading position...");

	unsigned loaded = 0x0;

	if (_bound_class)
	{
		const Ion::Node *node = &_bound_class->last(name);
		if (node->exists())
			loaded = read_position(position, *node, 0x0);
	}

	if (_bound_object)
	{
		const Ion::Node *node = &_bound_object->last(name);
		if (node->exists())
			loaded |= read_position(position, *node, loaded);
	}

	return loaded == 0x7;
}

bool IonPropertyLoader::load_rect(const StaticString &name, RectI *rect, bool update) const
{
	Y_LOG_TRACE("[Gui.Loader] Loading rect...");

	Integer elements[4] = {-1, -1, -1, -1};

	if (update)
	{
		elements[0] = rect->left();
		elements[1] = rect->top();
		elements[2] = rect->width();
		elements[3] = rect->height();
	}

	if (_bound_object)
	{
		const Ion::Node *node = &_bound_object->last(name);
		if (node->exists())
			read_rect(*node, elements);
	}

	if (_bound_class)
	{
		const Ion::Node *node = &_bound_class->last(name);
		if (node->exists())
			read_rect(*node, elements);
	}

	if (Y_UNLIKELY(elements[0] < 0 || elements[1] < 0 || elements[2] < 0 || elements[3] < 0))
		return false;

	*rect = RectI(elements[0], elements[1], elements[2], elements[3]);

	return true;
}

bool IonPropertyLoader::load_scaling(const StaticString &name, Scaling *scaling) const
{
	Y_LOG_TRACE("[Gui.Loader] Loading scaling...");

	if (_bound_object)
	{
		const Ion::Node *node = &_bound_object->last(name);
		if (node->exists() && load_scaling(scaling, *node))
			return true;
	}

	if (_bound_class)
	{
		const Ion::Node *node = &_bound_class->last(name);
		if (node->exists() && load_scaling(scaling, *node))
			return true;
	}

	return false;
}

bool IonPropertyLoader::load_size(const StaticString &name, Vector2f *size) const
{
	Y_LOG_TRACE("[Gui.Loader] Loading size...");

	unsigned loaded = 0x0;

	if (_bound_class)
	{
		const Ion::Node *node = &_bound_class->last(name);
		if (node->exists())
			loaded = read_size(size, *node, 0x0);
	}

	if (_bound_object)
	{
		const Ion::Node *node = &_bound_object->last(name);
		if (node->exists())
			loaded |= read_size(size, *node, loaded);
	}

	return loaded == 0x3;
}

SoundPtr IonPropertyLoader::load_sound(const StaticString &name) const
{
	Y_LOG_TRACE("[Gui.Loader] Loading sound...");

	if (_bound_object)
	{
		const Ion::Node *node = &_bound_object->last(name);
		if (node->exists())
			return load_sound(*node);
	}

	if (_bound_class)
	{
		const Ion::Node *node = &_bound_class->last(name);
		if (node->exists())
			return load_sound(*node);
	}

	return SoundPtr();
}

bool IonPropertyLoader::load_state(const StaticString &name, WidgetState *state) const
{
	Y_LOG_TRACE("[Gui.Loader] Loading state...");

	if (_bound_object)
	{
		const Ion::Node *node = &_bound_object->last(name);
		if (node->exists() && load_state(state, *node))
			return true;
	}

	if (_bound_class)
	{
		const Ion::Node *node = &_bound_class->last(name);
		if (node->exists() && load_state(state, *node))
			return true;
	}

	return false;
}

bool IonPropertyLoader::load_text(const StaticString &name, String *text) const
{
	Y_LOG_TRACE("[Gui.Loader] Loading text...");

	const StaticString *value;

	if (!_bound_object || !load_text(&value, *_bound_object, name))
	{
		return false;
	}

	*text = *value;
	return true;
}

bool IonPropertyLoader::load_texture(const StaticString &name, Texture2DPtr *texture) const
{
	Y_LOG_TRACE("[Gui.Loader] Loading texture...");

	if (_bound_object)
	{
		const Ion::Node *node = &_bound_object->last(name);
		if (node->exists() && load_texture(texture, *node, _texture_cache,
			Texture2D::TrilinearFilter | Texture2D::AnisotropicFilter))
		{
			return true;
		}
	}

	if (_bound_class)
	{
		const Ion::Node *node = &_bound_class->last(name);
		if (node->exists() && load_texture(texture, *node, _texture_cache,
			Texture2D::TrilinearFilter | Texture2D::AnisotropicFilter))
		{
			return true;
		}
	}

	return false;
}

void IonPropertyLoader::unbind()
{
	_bound_object = _object;
	_bound_class = _class;
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

		if (!values->get(&value))
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

bool IonPropertyLoader::load_margins(MarginsI *margins, const Ion::Node &node)
{
	Integer top    = -1;
	Integer right  = -1;
	Integer bottom = -1;
	Integer left   = -1;

	Ion::Node::ConstRange values = node.const_values();
	switch (values.size())
	{
	case 4:

		if (Y_UNLIKELY(!values.last().get(&left) || left < 0))
			return false;
		values.pop_last();
		// Fallthrough.

	case 3:

		if (Y_UNLIKELY(!values.last().get(&bottom) || bottom < 0))
			return false;
		values.pop_last();
		// Fallthrough.

	case 2:

		if (Y_UNLIKELY(!values.last().get(&right) || right < 0))
			return false;
		values.pop_last();
		// Fallthrough.

	case 1:

		if (Y_UNLIKELY(!values.last().get(&top) || top < 0))
			return false;
		break;

	default:

		return false;
	}

	if (right < 0)
		right = top;

	if (bottom < 0)
		bottom = top;

	if (left < 0)
		left = right;

	*margins = MarginsI(top, right, bottom, left);

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

	if (!values->get(&value))
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

SoundPtr IonPropertyLoader::load_sound(const Ion::Node &node)
{
	Ion::Node::ConstRange values = node.values();

	if (values.size() != 1)
		return SoundPtr();

	const StaticString *value;

	if (!values->get(&value))
		return SoundPtr();

	return Sound::open(*value);
}

bool IonPropertyLoader::load_state(WidgetState *state, const Ion::Node &node)
{
	Ion::Node::ConstRange values = node.values();

	if (values.size() != 1)
	{
		return false;
	}

	const StaticString *value;

	if (!values->get(&value))
	{
		return false;
	}
	else if (*value == S("normal"))
	{
		*state = WidgetState::Normal;
	}
	else if (*value == S("active"))
	{
		*state = WidgetState::Active;
	}
	else if (*value == S("pressed"))
	{
		*state = WidgetState::Pressed;
	}
	else if (*value == S("checked"))
	{
		*state = WidgetState::Checked;
	}
	else if (*value == S("disabled"))
	{
		*state = WidgetState::Disabled;
	}
	else
	{
		return false;
	}

	return true;
}

bool IonPropertyLoader::load_text(const StaticString **text, const Ion::Object &object, const StaticString &name)
{
	const Ion::Node &node = object.last(name);

	return node.size() == 1 && node.first()->get(text);
}

bool IonPropertyLoader::load_texture(Texture2DPtr *texture, const Ion::Node &node,
	TextureCache *texture_cache, Texture2D::Filter default_filter)
{
	Ion::Node::ConstRange values = node.values();

	if (values.is_empty() || values.size() > 4)
	{
		return false;
	}

	const StaticString *texture_name;

	if (!values->get(&texture_name))
	{
		return false;
	}

	values.pop_first();

	Texture2D::Filter filter = default_filter;

	Area     rect;
	MarginsI borders;

	if (!values.is_empty())
	{
		bool has_interpolation = false;
		bool has_anisotropy = false;

		for (; !values.is_empty(); values.pop_first())
		{
			const StaticString *value;

			if (!values->get(&value))
			{
				return false;
			}
			else if (*value == S("nearest"))
			{
				if (has_interpolation)
					return false;
				filter = Texture2D::NearestFilter | (filter & Texture2D::AnisotropicFilter);
				has_interpolation = true;
			}
			else if (*value == S("linear"))
			{
				if (has_interpolation)
					return false;
				filter = Texture2D::LinearFilter | (filter & Texture2D::AnisotropicFilter);
				has_interpolation = true;
			}
			else if (*value == S("bilinear"))
			{
				if (has_interpolation)
					return false;
				filter = Texture2D::BilinearFilter | (filter & Texture2D::AnisotropicFilter);
				has_interpolation = true;
			}
			else if (*value == S("trilinear"))
			{
				if (has_interpolation)
					return false;
				filter = Texture2D::TrilinearFilter | (filter & Texture2D::AnisotropicFilter);
				has_interpolation = true;
			}
			else if (*value == S("anisotropic"))
			{
				if (!has_interpolation || has_anisotropy)
					return false;
				filter = (filter & Texture2D::IsotropicFilterMask) | Texture2D::AnisotropicFilter;
				has_anisotropy = true;
			}
			else
			{
				return false;
			}
		}
	}

	Texture2DPtr result_texture = texture_cache->load_texture_2d(*texture_name);
	if (result_texture.is_null())
		return false;

	result_texture->set_filter(filter);
	*texture = result_texture;

	return true;
}

} // namespace Gui

} // namespace Yttrium
