#include "property_loader.h"

#include <Yttrium/ion/node.h>
#include <Yttrium/ion/value.h>
#include <Yttrium/renderer/texture_cache.h>

namespace Yttrium
{

namespace Gui
{

namespace
{

bool load_alignment(Alignment *alignment, const Ion::Node &node)
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

bool load_color(Vector4f *color, const Ion::Node &node, bool inherit)
{
	Ion::Node::ConstRange values = node.values();

	if (values.size() == 3)
	{
		if (!inherit)
		{
			color->a = 1.f;
		}
	}
	else if (values.size() != 4)
	{
		return false;
	}

	for (size_t i = 0; !values.is_empty(); values.pop_first(), ++i)
	{
		const StaticString *value;

		if (!values.first().get(&value))
		{
			return false;
		}
		else if (!value->is_empty())
		{
			if (!value->to_number(color->data() + i))
			{
				return false;
			}
		}
		else if (!inherit)
		{
			return false;
		}
	}

	return true;
}

bool load_position(Vector3f *position, const Ion::Node &node, bool inherit)
{
	Ion::Node::ConstRange values = node.values();

	if (values.size() == 2)
	{
		if (!inherit)
		{
			position->z = 0.f;
		}
	}
	else if (values.size() != 3)
	{
		return false;
	}

	for (size_t i = 0; !values.is_empty(); values.pop_first(), ++i)
	{
		const StaticString *value;

		if (!values.first().get(&value))
		{
			return false;
		}
		else if (!value->is_empty())
		{
			if (!value->to_number(position->data() + i))
			{
				return false;
			}
		}
		else if (!inherit)
		{
			return false;
		}
	}

	return true;
}

bool load_scaling(Scaling *scaling, const Ion::Node &node)
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

bool load_size(Vector2f *size, const Ion::Node &node, bool inherit)
{
	Ion::Node::ConstRange values = node.values();

	if (values.size() == 1)
	{
		if (!inherit)
		{
			size->y = 1.f;
		}
	}
	else if (values.size() != 2)
	{
		return false;
	}

	for (size_t i = 0; !values.is_empty(); values.pop_first(), ++i)
	{
		const StaticString *value;

		if (!values.first().get(&value))
		{
			return false;
		}
		else if (!value->is_empty())
		{
			if (!value->to_number(size->data() + i))
			{
				return false;
			}
		}
		else if (!inherit)
		{
			return false;
		}
	}

	return true;
}

bool load_texture(Texture2D *texture, const Ion::Node &node, TextureCache *texture_cache, Texture2D::Filter default_filter)
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

	*texture = texture_cache->load_texture_2d(*texture_name);

	if (!texture)
	{
		return false;
	}

	texture->set_filter(filter);

	return true;
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IonPropertyLoader::IonPropertyLoader()
{
	// TODO: Implement.
}

bool IonPropertyLoader::load_alignment(const StaticString &name, Alignment *alignment) const
{
	// TODO: Implement.

	return false;
}

bool IonPropertyLoader::load_color(const StaticString &name, Vector4f *color) const
{
	// TODO: Implement.

	return false;
}

bool IonPropertyLoader::load_position(const StaticString &name, Vector3f *color) const
{
	// TODO: Implement.

	return false;
}

bool IonPropertyLoader::load_scaling(const StaticString &name, Scaling *scaling) const
{
	// TODO: Implement.

	return false;
}

bool IonPropertyLoader::load_size(const StaticString &name, Vector2f *size) const
{
	// TODO: Implement.

	return false;
}

bool IonPropertyLoader::load_texture(const StaticString &name, Texture2D *texture) const
{
	// TODO: Implement.

	return false;
}

} // namespace Gui

} // namespace Yttrium
