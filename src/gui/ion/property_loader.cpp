#include "property_loader.h"

#include <yttrium/audio/sound.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/margins.h>
#include <yttrium/texture_cache.h>
#include "../gui.h"

#include <algorithm>

namespace Yttrium
{

namespace
{

unsigned read_color(Vector4 *color, const IonNode &node, unsigned inherit)
{
	unsigned result = 0;

	auto&& values = node.values();

	if (!values.is_empty())
	{
		const size_t items = std::min<size_t>(values.size(), 4);

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

unsigned read_position(Vector2 *position, const IonNode &node)
{
	unsigned result = 0;

	auto&& values = node.values();

	if (!values.is_empty())
	{
		const size_t items = std::min<size_t>(values.size(), 2);

		for (size_t i = 0; i < items; values.pop_first(), ++i)
		{
			const StaticString *value;
			if (values->get(&value) && value->to_number(position->data() + i))
				result |= 1 << i;
		}
	}

	return result;
}

void read_rect(int32_t elements[4], const IonNode& node)
{
	auto&& values = node.values();

	for (int i = 0; i < 4; ++i)
	{
		if (values.is_empty())
			break;

		const StaticString* value;
		if (values->get(&value))
		{
			int32_t number;
			if (value->to_number(&number) && number >= i >> 1) // 'number' must be not less than {0, 0, 1, 1}.
				elements[i] = number;
		}
		
		values.pop_first();
	}
}

unsigned read_size(Vector2 *size, const IonNode &node, unsigned inherit)
{
	unsigned result = 0;

	auto&& values = node.values();

	if (!values.is_empty())
	{
		const size_t items = std::min<size_t>(values.size(), 2);

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

GuiIonPropertyLoader::GuiIonPropertyLoader(const IonObject* object, const IonObject* class_, GuiImpl& gui)
	: _object(object)
	, _class(class_)
	, _gui(gui)
	, _texture_cache(gui.texture_cache())
	, _bound_object(_object)
	, _bound_class(_class)
{
}

void GuiIonPropertyLoader::bind(const StaticString& name)
{
	_bound_object = nullptr;

	const IonNode *node = &_object->last(name);
	if (node->exists() && node->size() != 1)
		node->first()->get(&_bound_object);

	_bound_class = nullptr;

	if (_class)
	{
		const IonNode *node = &_class->last(name);
		if (node->size() == 1)
			node->first()->get(&_bound_class);
	}
}

bool GuiIonPropertyLoader::load_alignment(const StaticString& name, unsigned* alignment) const
{
	if (_bound_object)
	{
		const IonNode *node = &_bound_object->last(name);
		if (node->exists() && load_alignment(alignment, *node))
			return true;
	}

	if (_bound_class)
	{
		const IonNode *node = &_bound_class->last(name);
		if (node->exists() && load_alignment(alignment, *node))
			return true;
	}

	return false;
}

bool GuiIonPropertyLoader::load_color(const StaticString& name, Vector4* color) const
{
	unsigned loaded = 0x0;

	if (_bound_class)
	{
		const IonNode *node = &_bound_class->last(name);
		if (node->exists())
			loaded = read_color(color, *node, 0x0);
	}

	if (_bound_object)
	{
		const IonNode *node = &_bound_object->last(name);
		if (node->exists())
			loaded |= read_color(color, *node, loaded);
	}

	return loaded == 0xF;
}

bool GuiIonPropertyLoader::load_font(const StaticString& name, TextureFont* font, SharedPtr<Texture2D>* texture) const
{
	const StaticString *font_name;

	const IonNode *node = _bound_object ? &_bound_object->last(name) : nullptr;

	if (!node->exists() || node->size() != 1 || !node->first()->get(&font_name))
	{
		if (!_bound_class)
			return false;

		node = &_bound_class->last(name);

		if (!node->exists() || node->size() != 1 || !node->first()->get(&font_name))
			return false;
	}

	const GuiImpl::FontDesc *font_desc = _gui.font(*font_name);

	if (!font_desc)
		return false;

	*font = font_desc->font;
	*texture = font_desc->texture;

	return true;
}

bool GuiIonPropertyLoader::load_margins(const StaticString& name, Margins* margins) const
{
	if (_bound_object)
	{
		const IonNode *node = &_bound_object->last(name);
		if (node->exists() && load_margins(margins, *node))
			return true;
	}

	if (_bound_class)
	{
		const IonNode *node = &_bound_class->last(name);
		if (node->exists() && load_margins(margins, *node))
			return true;
	}

	return false;
}

bool GuiIonPropertyLoader::load_position(const StaticString& name, Vector2* position) const
{
	unsigned loaded = 0x0;

	if (_bound_class)
	{
		const IonNode *node = &_bound_class->last(name);
		if (node->exists())
			loaded = read_position(position, *node);
	}

	if (_bound_object)
	{
		const IonNode *node = &_bound_object->last(name);
		if (node->exists())
			loaded |= read_position(position, *node);
	}

	return loaded == 0x3;
}

bool GuiIonPropertyLoader::load_rect(const StaticString& name, Rect* rect, bool update) const
{
	int32_t elements[4] = {-1, -1, -1, -1};

	if (update)
	{
		elements[0] = rect->left();
		elements[1] = rect->top();
		elements[2] = rect->width();
		elements[3] = rect->height();
	}

	if (_bound_object)
	{
		const IonNode *node = &_bound_object->last(name);
		if (node->exists())
			read_rect(elements, *node);
	}

	if (_bound_class)
	{
		const IonNode *node = &_bound_class->last(name);
		if (node->exists())
			read_rect(elements, *node);
	}

	if (!update && (elements[0] < 0 || elements[1] < 0 || elements[2] < 0 || elements[3] < 0))
		return false;

	*rect = Rect(elements[0], elements[1], elements[2], elements[3]);

	return true;
}

bool GuiIonPropertyLoader::load_scaling(const StaticString& name, Scaling* scaling) const
{
	if (_bound_object)
	{
		const IonNode *node = &_bound_object->last(name);
		if (node->exists() && load_scaling(scaling, *node))
			return true;
	}

	if (_bound_class)
	{
		const IonNode *node = &_bound_class->last(name);
		if (node->exists() && load_scaling(scaling, *node))
			return true;
	}

	return false;
}

bool GuiIonPropertyLoader::load_size(const StaticString& name, Vector2* size) const
{
	unsigned loaded = 0x0;

	if (_bound_class)
	{
		const IonNode *node = &_bound_class->last(name);
		if (node->exists())
			loaded = read_size(size, *node, 0x0);
	}

	if (_bound_object)
	{
		const IonNode *node = &_bound_object->last(name);
		if (node->exists())
			loaded |= read_size(size, *node, loaded);
	}

	return loaded == 0x3;
}

SharedPtr<Sound> GuiIonPropertyLoader::load_sound(const StaticString& name) const
{
	if (_bound_object)
	{
		const IonNode *node = &_bound_object->last(name);
		if (node->exists())
			return load_sound(*node);
	}

	if (_bound_class)
	{
		const IonNode *node = &_bound_class->last(name);
		if (node->exists())
			return load_sound(*node);
	}

	return {};
}

bool GuiIonPropertyLoader::load_state(const StaticString& name, WidgetState* state) const
{
	if (_bound_object)
	{
		const IonNode *node = &_bound_object->last(name);
		if (node->exists() && load_state(state, *node))
			return true;
	}

	if (_bound_class)
	{
		const IonNode *node = &_bound_class->last(name);
		if (node->exists() && load_state(state, *node))
			return true;
	}

	return false;
}

bool GuiIonPropertyLoader::load_text(const StaticString& name, String* text) const
{
	const StaticString *value;

	if (!_bound_object || !load_text(&value, *_bound_object, name))
	{
		return false;
	}

	*text = *value;
	return true;
}

bool GuiIonPropertyLoader::load_texture(const StaticString& name, SharedPtr<Texture2D>* texture) const
{
	if (_bound_object)
	{
		const IonNode *node = &_bound_object->last(name);
		if (node->exists() && load_texture(texture, *node, _texture_cache,
			Texture2D::TrilinearFilter | Texture2D::AnisotropicFilter))
		{
			return true;
		}
	}

	if (_bound_class)
	{
		const IonNode *node = &_bound_class->last(name);
		if (node->exists() && load_texture(texture, *node, _texture_cache,
			Texture2D::TrilinearFilter | Texture2D::AnisotropicFilter))
		{
			return true;
		}
	}

	return false;
}

bool GuiIonPropertyLoader::load_translatable(const StaticString& name, String* text) const
{
	if (!_bound_object)
		return false;

	const auto& node = _bound_object->last(name);
	if (node.size() != 1)
		return false;

	const auto& value = *node.first();
	if (value.type() == IonValue::Type::String)
	{
		*text = value.string();
		return true;
	}

	if (value.type() != IonValue::Type::Object)
		return false;

	const auto& tr_object = *value.object();
	if (tr_object.size() != 1)
		return false;

	const auto& tr_node = *tr_object.begin();
	if (tr_node.name() != S("tr") || tr_node.size() != 1)
		return false;

	const auto& tr_value = *tr_node.first();
	if (tr_value.type() != IonValue::Type::String)
		return false;

	*text = tr_value.string(); // TODO: Load translation.
	return true;
}

void GuiIonPropertyLoader::unbind()
{
	_bound_object = _object;
	_bound_class = _class;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiIonPropertyLoader::load_alignment(unsigned* alignment, const IonNode& node)
{
	// TODO: Inheritance.

	auto&& values = node.values();

	if (values.is_empty() || values.size() > 2)
		return false;

	unsigned result = CenterAlignment;
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
				return false;
			is_centered = true;
		}
		else if (*value == S("left"))
		{
			if (result & HorizontalAlignmentMask)
				return false;
			result |= LeftAlignment;
		}
		else if (*value == S("right"))
		{
			if (result & HorizontalAlignmentMask)
				return false;
			result |= RightAlignment;
		}
		else if (*value == S("top"))
		{
			if (result & VerticalAlignmentMask)
				return false;
			result |= TopAlignment;
		}
		else if (*value == S("bottom"))
		{
			if (result & VerticalAlignmentMask)
				return false;
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

bool GuiIonPropertyLoader::load_margins(Margins* margins, const IonNode& node)
{
	int32_t top    = -1;
	int32_t right  = -1;
	int32_t bottom = -1;
	int32_t left   = -1;

	auto&& values = node.values();
	switch (values.size())
	{
	case 4:

		if (!values.last().get(&left) || left < 0)
			return false;
		values.pop_last();
		// Fallthrough.

	case 3:

		if (!values.last().get(&bottom) || bottom < 0)
			return false;
		values.pop_last();
		// Fallthrough.

	case 2:

		if (!values.last().get(&right) || right < 0)
			return false;
		values.pop_last();
		// Fallthrough.

	case 1:

		if (!values.last().get(&top) || top < 0)
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

	*margins = Margins(top, right, bottom, left);

	return true;
}

bool GuiIonPropertyLoader::load_scaling(Scaling* scaling, const IonNode& node)
{
	const auto& values = node.values();

	if (values.size() != 1)
		return false;

	const StaticString* value;
	if (!values->get(&value))
		return false;

	if (*value == S("fit"))
		*scaling = Scaling::Fit;
	else if (*value == S("max"))
		*scaling = Scaling::Max;
	else if (*value == S("min"))
		*scaling = Scaling::Min;
	else if (*value == S("stretch"))
		*scaling = Scaling::Stretch;
	else
		return false;

	return true;
}

bool GuiIonPropertyLoader::load_size(Vector2* size, const IonNode& node)
{
	return read_size(size, node, 0) == 3;
}

SharedPtr<Sound> GuiIonPropertyLoader::load_sound(const IonNode& node)
{
	const auto& values = node.values();

	if (values.size() != 1)
		return {};

	const StaticString *value;

	if (!values->get(&value))
		return {};

	return Sound::create(*value);
}

bool GuiIonPropertyLoader::load_state(WidgetState* state, const IonNode &node)
{
	const auto& values = node.values();

	if (values.size() != 1)
		return false;

	const StaticString* value;
	if (!values->get(&value))
		return false;

	if (*value == S("normal"))
		*state = WidgetState::Normal;
	else if (*value == S("active"))
		*state = WidgetState::Active;
	else if (*value == S("pressed"))
		*state = WidgetState::Pressed;
	else if (*value == S("checked"))
		*state = WidgetState::Checked;
	else if (*value == S("disabled"))
		*state = WidgetState::Disabled;
	else
		return false;

	return true;
}

bool GuiIonPropertyLoader::load_text(const StaticString** text, const IonObject& object, const StaticString& name)
{
	const IonNode& node = object.last(name);
	return node.size() == 1 && node.first()->get(text);
}

bool GuiIonPropertyLoader::load_texture(SharedPtr<Texture2D>* texture, const IonNode& node,
	TextureCache& texture_cache, Texture2D::Filter default_filter)
{
	auto&& values = node.values();

	if (values.is_empty() || values.size() > 4)
		return false;

	const StaticString* texture_name;
	if (!values->get(&texture_name))
		return false;

	values.pop_first();

	Texture2D::Filter filter = default_filter;

	Rect     rect;
	Margins borders;

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

	const auto result_texture = texture_cache.load_texture_2d(*texture_name);
	if (!result_texture)
		return false;

	result_texture->set_filter(filter);
	*texture = result_texture;

	return true;
}

} // namespace Yttrium
