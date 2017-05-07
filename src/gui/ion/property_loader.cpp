#include "property_loader.h"

#include <yttrium/audio/sound.h>
#include <yttrium/exceptions.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/math/color.h>
#include <yttrium/math/margins.h>
#include <yttrium/math/rect.h>
#include <yttrium/resource_loader.h>
#include <yttrium/utils.h>
#include "../actions.h"
#include "../gui.h"

#include <array>

namespace
{
	using namespace Yttrium;

	template <std::size_t N>
	void read_floats(std::array<float, N>& elements, const IonNode& node)
	{
		auto&& values = node.values();
		for (auto& element : elements)
		{
			if (values.is_empty())
				break;
			std::string_view value;
			if (values->get(value))
			{
				float number;
				if (strings::to_number(value, number))
					element = number;
			}
			values.pop_first();
		}
	}

	template <typename T, std::size_t N>
	std::size_t read_array(std::array<T, N>& elements, const IonNode& node)
	{
		if (node.size() > N)
			throw GuiDataError{"Bad '", node.name(), "'"};
		auto&& values = node.values();
		for (std::size_t i = 0; i < N; ++i)
		{
			if (values.is_empty())
				return i;
			std::string_view value;
			if (values->get(value))
			{
				T number;
				if (!strings::to_number(value, number))
					throw GuiDataError{"Bad '", node.name(), "'"};
				elements[i] = number;
			}
			values.pop_first();
		}
		return N;
	}

	void read_color(Color4f& color, const IonNode& node)
	{
		std::array<float, 4> elements;
		switch (read_array(elements, node))
		{
		case 3:
			elements[3] = 1;
		case 4:
			break;
		default:
			throw GuiDataError{"Bad '", node.name(), "'"};
		}
		color = {elements[0], elements[1], elements[2], elements[3]};
	}
}

namespace Yttrium
{
	GuiIonPropertyLoader::GuiIonPropertyLoader(const IonObject* object, const IonObject* class_, GuiPrivate& gui)
		: _object{object}
		, _class{class_}
		, _gui{gui}
		, _bound_object{_object}
		, _bound_class{_class}
	{
	}

	void GuiIonPropertyLoader::set_default_font_name(const std::string* name)
	{
		_default_font_name = name;
	}

	void GuiIonPropertyLoader::bind(std::string_view name)
	{
		_bound_object = nullptr;

		const IonNode& node = _object->last(name);
		if (node.exists() && node.size() == 1)
			node.first()->get(&_bound_object);

		_bound_class = nullptr;

		if (_class)
		{
			const IonNode& class_node = _class->last(name);
			if (class_node.size() == 1)
				class_node.first()->get(&_bound_class);
		}
	}

	bool GuiIonPropertyLoader::load(std::string_view name, float& value) const
	{
		if (_bound_object)
		{
			const IonNode& node = _bound_object->last(name);
			if (node.exists() && load(value, node))
				return true;
		}

		if (_bound_class)
		{
			const IonNode& node = _bound_class->last(name);
			if (node.exists() && load(value, node))
				return true;
		}

		return false;
	}

	GuiActions GuiIonPropertyLoader::load_actions(std::string_view name) const
	{
		if (_bound_object)
		{
			const IonNode& node = _bound_object->last(name);
			if (node.exists())
				return load_actions(node);
		}

		if (_bound_class)
		{
			const IonNode& node = _bound_class->last(name);
			if (node.exists())
				return load_actions(node);
		}

		return {};
	}

	bool GuiIonPropertyLoader::load_alignment(std::string_view name, unsigned* alignment) const
	{
		if (_bound_object)
		{
			const IonNode& node = _bound_object->last(name);
			if (node.exists() && load_alignment(alignment, node))
				return true;
		}

		if (_bound_class)
		{
			const IonNode& node = _bound_class->last(name);
			if (node.exists() && load_alignment(alignment, node))
				return true;
		}

		return false;
	}

	bool GuiIonPropertyLoader::load_color(std::string_view name, Color4f* color) const
	{
		if (_bound_object)
		{
			const IonNode& node = _bound_object->last(name);
			if (node.exists())
			{
				::read_color(*color, node);
				return true;
			}
		}

		if (_bound_class)
		{
			const IonNode& node = _bound_class->last(name);
			if (node.exists())
			{
				::read_color(*color, node);
				return true;
			}
		}

		return false;
	}

	void GuiIonPropertyLoader::load_font(std::string_view name, std::shared_ptr<const TextureFont>* font, std::shared_ptr<const Texture2D>* texture) const
	{
		std::string_view font_name;

		if (_bound_object)
		{
			const IonNode& node = _bound_object->last(name);
			if (node.exists() && node.size() == 1)
				node.first()->get(font_name);
		}

		if (!font_name.data() && _bound_class)
		{
			const IonNode& node = _bound_class->last(name);
			if (node.exists() && node.size() == 1)
				node.first()->get(font_name);
		}

		const auto font_desc = font_name.data() ? _gui.font(strings::from_view(font_name)) : (_default_font_name ? _gui.font(*_default_font_name) : nullptr);
		if (!font_desc)
			throw GuiDataError{"Bad font ", font_name};

		*font = font_desc->font;
		*texture = font_desc->texture;
	}

	bool GuiIonPropertyLoader::load_margins(std::string_view name, Margins& margins) const
	{
		if (_bound_object)
		{
			const IonNode& node = _bound_object->last(name);
			if (node.exists())
			{
				load_margins(margins, node);
				return true;
			}
		}

		if (_bound_class)
		{
			const IonNode& node = _bound_class->last(name);
			if (node.exists())
			{
				load_margins(margins, node);
				return true;
			}
		}

		return false;
	}

	bool GuiIonPropertyLoader::load_rect(std::string_view name, RectF& rect, bool update) const
	{
		std::array<float, 4> elements = {-1, -1, -1, -1};

		if (update)
		{
			elements[0] = rect.left();
			elements[1] = rect.top();
			elements[2] = rect.width();
			elements[3] = rect.height();
		}

		if (_bound_object)
		{
			const IonNode& node = _bound_object->last(name);
			if (node.exists())
				::read_floats(elements, node);
		}

		if (_bound_class)
		{
			const IonNode& node = _bound_class->last(name);
			if (node.exists())
				::read_floats(elements, node);
		}

		if (elements[0] < 0 || elements[1] < 0 || elements[2] <= 0 || elements[3] <= 0)
			return false;

		rect = RectF({elements[0], elements[1]}, SizeF(elements[2], elements[3]));

		return true;
	}

	std::shared_ptr<const Sound> GuiIonPropertyLoader::load_sound(std::string_view name) const
	{
		const auto load_sound_node = [this](const IonNode& node) -> std::shared_ptr<const Sound>
		{
			const auto& values = node.values();
			if (values.size() != 1)
				return {};
			std::string_view value;
			if (!values->get(value))
				return {};
			return _gui.resource_loader().load_sound(value);
		};

		if (_bound_object)
		{
			const IonNode& node = _bound_object->last(name);
			if (node.exists())
				return load_sound_node(node);
		}

		if (_bound_class)
		{
			const IonNode& node = _bound_class->last(name);
			if (node.exists())
				return load_sound_node(node);
		}

		return {};
	}

	bool GuiIonPropertyLoader::load_state(std::string_view name, WidgetState* state) const
	{
		if (_bound_object)
		{
			const IonNode& node = _bound_object->last(name);
			if (node.exists() && load_state(state, node))
				return true;
		}

		if (_bound_class)
		{
			const IonNode& node = _bound_class->last(name);
			if (node.exists() && load_state(state, node))
				return true;
		}

		return false;
	}

	bool GuiIonPropertyLoader::load_text(std::string_view name, std::string& text) const
	{
		std::string_view value;
		if (!_bound_object || !load_text(value, _bound_object->last(name)))
			return false;
		strings::assign_view(text, value);
		return true;
	}

	bool GuiIonPropertyLoader::load_texture(std::string_view name, std::shared_ptr<const Texture2D>& texture, Texture2D::Filter& filter) const
	{
		if (_bound_object)
		{
			const IonNode& node = _bound_object->last(name);
			if (node.exists() && load_texture(texture, filter, node, _gui.resource_loader()))
				return true;
		}

		if (_bound_class)
		{
			const IonNode& node = _bound_class->last(name);
			if (node.exists() && load_texture(texture, filter, node, _gui.resource_loader()))
				return true;
		}

		return false;
	}

	bool GuiIonPropertyLoader::load_translatable(std::string_view name, std::string& text) const
	{
		// TODO: Enable translation for classes.

		if (!_bound_object)
			return false;

		const auto& node = _bound_object->last(name);
		if (node.size() != 1)
			return false;

		const auto& value = *node.first();
		if (value.type() == IonValue::Type::String)
		{
			strings::assign_view(text, value.string());
			return true;
		}

		if (value.type() != IonValue::Type::Object)
			return false;

		const auto& tr_object = *value.object();
		if (tr_object.size() != 1)
			return false;

		const auto& tr_node = *tr_object.begin();
		if (tr_node.name() != "tr" || tr_node.size() != 1)
			return false;

		const auto& tr_value = *tr_node.first();
		if (tr_value.type() != IonValue::Type::String)
			return false;

		text = _gui.translate(tr_value.string());
		return true;
	}

	void GuiIonPropertyLoader::unbind()
	{
		_bound_object = _object;
		_bound_class = _class;
	}

	bool GuiIonPropertyLoader::load(float& value, const IonNode& node)
	{
		std::array<float, 1> elements{};
		if (::read_array(elements, node) != 1)
			throw GuiDataError{"Bad '", node.name(), "'"};
		value = elements[0];
		return true;
	}

	GuiActions GuiIonPropertyLoader::load_actions(const IonObject& object)
	{
		GuiActions actions{object.size()};
		for (const auto& node : object)
		{
			if (node.name() == "call")
			{
				std::string_view code;
				if (!GuiIonPropertyLoader::load_text(code, node))
					throw GuiDataError{"Bad action '", node.name(), "'"};
				actions.add<GuiAction_Call>(code);
			}
			else if (node.name() == "enter")
			{
				std::string_view screen;
				if (!GuiIonPropertyLoader::load_text(screen, node))
					throw GuiDataError{"Bad action '", node.name(), "'"};
				actions.add<GuiAction_Enter>(screen);
			}
			else if (node.name() == "quit")
			{
				actions.add<GuiAction_Quit>();
			}
			else if (node.name() == "return")
			{
				actions.add<GuiAction_Return>();
			}
			else if (node.name() == "return_to")
			{
				std::string_view screen;
				if (!GuiIonPropertyLoader::load_text(screen, node))
					throw GuiDataError{"Bad action '", node.name(), "'"};
				actions.add<GuiAction_ReturnTo>(screen);
			}
		}
		return actions;
	}

	GuiActions GuiIonPropertyLoader::load_actions(const IonNode& node)
	{
		if (node.size() != 1 || node.first()->type() != IonValue::Type::Object)
			throw GuiDataError{"Bad action sequence '", node.name(), "'"};
		return load_actions(*node.first()->object());
	}

	bool GuiIonPropertyLoader::load_alignment(unsigned* alignment, const IonNode& node)
	{
		auto&& values = node.values();

		if (values.is_empty() || values.size() > 2)
			return false;

		unsigned result = CenterAlignment;
		bool is_centered = false;

		for (; !values.is_empty(); values.pop_first())
		{
			std::string_view value;
			if (!values->get(value))
				return false;

			if (value == "center")
			{
				if (is_centered)
					return false;
				is_centered = true;
			}
			else if (value == "left")
			{
				if (result & HorizontalAlignmentMask)
					return false;
				result |= LeftAlignment;
			}
			else if (value == "right")
			{
				if (result & HorizontalAlignmentMask)
					return false;
				result |= RightAlignment;
			}
			else if (value == "top")
			{
				if (result & VerticalAlignmentMask)
					return false;
				result |= TopAlignment;
			}
			else if (value == "bottom")
			{
				if (result & VerticalAlignmentMask)
					return false;
				result |= BottomAlignment;
			}
			else
				return false;
		}

		*alignment = result;
		return true;
	}

	void GuiIonPropertyLoader::load_margins(Margins& margins, const IonNode& node)
	{
		std::array<int32_t, 4> elements;
		switch (::read_array(elements, node))
		{
		case 1:
			elements[1] = elements[0];
		case 2:
			elements[2] = elements[0];
		case 3:
			elements[3] = elements[1];
		case 4:
			break;
		default:
			throw GuiDataError{"Bad '", node.name(), "'"};
		}
		margins = {elements[0], elements[1], elements[2], elements[3]};
	}

	void GuiIonPropertyLoader::load_size(SizeF& size, const IonNode& node)
	{
		std::array<float, 2> elements;
		if (::read_array(elements, node) != 2
			|| elements[0] <= 0
			|| elements[1] <= 0)
			throw GuiDataError{"Bad '", node.name(), "'"};
		size = {elements[0], elements[1]};
	}

	bool GuiIonPropertyLoader::load_state(WidgetState* state, const IonNode& node)
	{
		const auto& values = node.values();

		if (values.size() != 1)
			return false;

		std::string_view value;
		if (!values->get(value))
			return false;

		if (value == "normal")
			*state = WidgetState::Normal;
		else if (value == "active")
			*state = WidgetState::Active;
		else if (value == "pressed")
			*state = WidgetState::Pressed;
		else if (value == "checked")
			*state = WidgetState::Checked;
		else if (value == "disabled")
			*state = WidgetState::Disabled;
		else
			return false;

		return true;
	}

	bool GuiIonPropertyLoader::load_text(std::string_view& text, const IonNode& node)
	{
		return node.size() == 1 && node.first()->get(text);
	}

	bool GuiIonPropertyLoader::load_texture(std::shared_ptr<const Texture2D>& texture, Texture2D::Filter& filter, const IonNode& node, ResourceLoader& resource_loader)
	{
		auto&& values = node.values();

		if (values.is_empty() || values.size() > 4)
			return false;

		std::string_view texture_name;
		if (!values->get(texture_name))
			return false;

		values.pop_first();

		Rect rect;
		Margins borders;

		if (!values.is_empty())
		{
			bool has_interpolation = false;
			bool has_anisotropy = false;

			for (; !values.is_empty(); values.pop_first())
			{
				std::string_view value;
				if (!values->get(value))
					return false;

				if (value == "nearest")
				{
					if (has_interpolation)
						return false;
					filter = static_cast<Texture2D::Filter>(Texture2D::NearestFilter | (filter & Texture2D::AnisotropicFilter));
					has_interpolation = true;
				}
				else if (value == "linear")
				{
					if (has_interpolation)
						return false;
					filter = static_cast<Texture2D::Filter>(Texture2D::LinearFilter | (filter & Texture2D::AnisotropicFilter));
					has_interpolation = true;
				}
				else if (value == "bilinear")
				{
					if (has_interpolation)
						return false;
					filter = static_cast<Texture2D::Filter>(Texture2D::BilinearFilter | (filter & Texture2D::AnisotropicFilter));
					has_interpolation = true;
				}
				else if (value == "trilinear")
				{
					if (has_interpolation)
						return false;
					filter = static_cast<Texture2D::Filter>(Texture2D::TrilinearFilter | (filter & Texture2D::AnisotropicFilter));
					has_interpolation = true;
				}
				else if (value == "anisotropic")
				{
					if (!has_interpolation || has_anisotropy)
						return false;
					filter = static_cast<Texture2D::Filter>((filter & Texture2D::IsotropicFilterMask) | Texture2D::AnisotropicFilter);
					has_anisotropy = true;
				}
				else
					return false;
			}
		}

		const auto result_texture = resource_loader.load_texture_2d(texture_name);
		if (!result_texture)
			return false;

		texture = result_texture;

		return true;
	}
}
