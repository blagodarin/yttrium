#include "property_loader.h"

#include <yttrium/audio/sound.h>
#include <yttrium/exceptions.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/math/margins.h>
#include <yttrium/math/rect.h>
#include <yttrium/math/vector.h>
#include <yttrium/resources/resource_loader.h>
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

			const StaticString* value;
			if (values->get(&value))
			{
				float number;
				if (value->to_number(number))
					element = number;
			}
			values.pop_first();
		}
	}

	template <typename T, std::size_t N>
	std::size_t read_array(std::array<T, N>& elements, const IonNode& node)
	{
		if (node.size() > N)
			throw GuiDataError("Bad '", node.name(), "'");
		auto&& values = node.values();
		for (std::size_t i = 0; i < N; ++i)
		{
			if (values.is_empty())
				return i;
			const StaticString* value;
			if (values->get(&value))
			{
				T number;
				if (!value->to_number(number))
					throw GuiDataError("Bad '", node.name(), "'");
				elements[i] = number;
			}
			values.pop_first();
		}
		return N;
	}

	void read_color(Vector4& color, const IonNode& node)
	{
		std::array<float, 4> elements;
		switch (read_array(elements, node))
		{
		case 3:
			elements[3] = 1;
		case 4:
			break;
		default:
			throw GuiDataError("Bad '", node.name(), "'");
		}
		color = { elements[0], elements[1], elements[2], elements[3] };
	}
}

namespace Yttrium
{
	GuiIonPropertyLoader::GuiIonPropertyLoader(const IonObject* object, const IonObject* class_, GuiPrivate& gui)
		: _object(object)
		, _class(class_)
		, _gui(gui)
		, _bound_object(_object)
		, _bound_class(_class)
	{
	}

	void GuiIonPropertyLoader::set_default_font_name(const std::string* name)
	{
		_default_font_name = name;
	}

	void GuiIonPropertyLoader::bind(const StaticString& name)
	{
		_bound_object = nullptr;

		const IonNode& node = _object->last(name);
		if (node.exists() && node.size() == 1)
			node.first()->get(&_bound_object);

		_bound_class = nullptr;

		if (_class)
		{
			const IonNode& node = _class->last(name);
			if (node.size() == 1)
				node.first()->get(&_bound_class);
		}
	}

	bool GuiIonPropertyLoader::load(const StaticString& name, float& value) const
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

	GuiActions GuiIonPropertyLoader::load_actions(const StaticString& name) const
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

	bool GuiIonPropertyLoader::load_alignment(const StaticString& name, unsigned* alignment) const
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

	bool GuiIonPropertyLoader::load_color(const StaticString& name, Vector4* color) const
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

	void GuiIonPropertyLoader::load_font(const StaticString& name, std::shared_ptr<const TextureFont>* font, std::shared_ptr<const Texture2D>* texture) const
	{
		const StaticString* font_name = nullptr;

		if (_bound_object)
		{
			const IonNode& node = _bound_object->last(name);
			if (node.exists() && node.size() == 1)
				node.first()->get(&font_name);
		}

		if (!font_name && _bound_class)
		{
			const IonNode& node = _bound_class->last(name);
			if (node.exists() && node.size() == 1)
				node.first()->get(&font_name);
		}

		const auto font_desc = font_name ? _gui.font(font_name->to_std()) : (_default_font_name ? _gui.font(*_default_font_name) : nullptr);
		if (!font_desc)
			throw GuiDataError("Bad font ", *font_name);

		*font = font_desc->font;
		*texture = font_desc->texture;
	}

	bool GuiIonPropertyLoader::load_margins(const StaticString& name, Margins& margins) const
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

	bool GuiIonPropertyLoader::load_rect(const StaticString& name, RectF& rect, bool update) const
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

	std::shared_ptr<const Sound> GuiIonPropertyLoader::load_sound(const StaticString& name) const
	{
		const auto load_sound_node = [this](const IonNode& node) -> std::shared_ptr<const Sound>
		{
			const auto& values = node.values();
			if (values.size() != 1)
				return {};
			const StaticString* value;
			if (!values->get(&value))
				return {};
			return _gui.resource_loader().load_sound(*value);
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

	bool GuiIonPropertyLoader::load_state(const StaticString& name, WidgetState* state) const
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

	bool GuiIonPropertyLoader::load_text(const StaticString& name, std::string& text) const
	{
		const StaticString* value;
		if (!_bound_object || !load_text(&value, _bound_object->last(name)))
			return false;
		text.assign(value->text(), value->size());
		return true;
	}

	bool GuiIonPropertyLoader::load_texture(const StaticString& name, std::shared_ptr<const Texture2D>& texture, Texture2D::Filter& filter) const
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

	bool GuiIonPropertyLoader::load_translatable(const StaticString& name, std::string& text) const
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
			const auto& untranslated = value.string();
			text.assign(untranslated.text(), untranslated.size());
			return true;
		}

		if (value.type() != IonValue::Type::Object)
			return false;

		const auto& tr_object = *value.object();
		if (tr_object.size() != 1)
			return false;

		const auto& tr_node = *tr_object.begin();
		if (tr_node.name() != "tr"_s || tr_node.size() != 1)
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
		std::array<float, 1> elements;
		if (::read_array(elements, node) != 1)
			throw GuiDataError("Bad '", node.name(), "'");
		value = elements[0];
		return true;
	}

	GuiActions GuiIonPropertyLoader::load_actions(const IonObject& object)
	{
		GuiActions actions(object.size());
		for (const auto& node : object)
		{
			if (node.name() == "call"_s)
			{
				const StaticString* code;
				if (!GuiIonPropertyLoader::load_text(&code, node))
					throw GuiDataError("Bad action '"_s, node.name(), "'"_s);
				actions.add<GuiAction_Call>(code->to_std());
			}
			else if (node.name() == "enter"_s)
			{
				const StaticString* screen;
				if (!GuiIonPropertyLoader::load_text(&screen, node))
					throw GuiDataError("Bad action '"_s, node.name(), "'"_s);
				actions.add<GuiAction_Enter>(screen->to_std());
			}
			else if (node.name() == "quit"_s)
			{
				actions.add<GuiAction_Quit>();
			}
			else if (node.name() == "return"_s)
			{
				actions.add<GuiAction_Return>();
			}
			else if (node.name() == "return_to"_s)
			{
				const StaticString* screen;
				if (!GuiIonPropertyLoader::load_text(&screen, node))
					throw GuiDataError("Bad action '"_s, node.name(), "'"_s);
				actions.add<GuiAction_ReturnTo>(screen->to_std());
			}
		}
		return actions;
	}

	GuiActions GuiIonPropertyLoader::load_actions(const IonNode& node)
	{
		if (node.size() != 1 || node.first()->type() != IonValue::Type::Object)
			throw GuiDataError("Bad action sequence '"_s, node.name(), "'"_s);
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
			const StaticString* value;

			if (!values->get(&value))
			{
				return false;
			}
			else if (*value == "center"_s)
			{
				if (is_centered)
					return false;
				is_centered = true;
			}
			else if (*value == "left"_s)
			{
				if (result & HorizontalAlignmentMask)
					return false;
				result |= LeftAlignment;
			}
			else if (*value == "right"_s)
			{
				if (result & HorizontalAlignmentMask)
					return false;
				result |= RightAlignment;
			}
			else if (*value == "top"_s)
			{
				if (result & VerticalAlignmentMask)
					return false;
				result |= TopAlignment;
			}
			else if (*value == "bottom"_s)
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
			throw GuiDataError("Bad '", node.name(), "'");
		}
		margins = {elements[0], elements[1], elements[2], elements[3]};
	}

	void GuiIonPropertyLoader::load_size(SizeF& size, const IonNode& node)
	{
		std::array<float, 2> elements;
		if (::read_array(elements, node) != 2
			|| elements[0] <= 0
			|| elements[1] <= 0)
			throw GuiDataError("Bad '", node.name(), "'");
		size = {elements[0], elements[1]};
	}

	bool GuiIonPropertyLoader::load_state(WidgetState* state, const IonNode& node)
	{
		const auto& values = node.values();

		if (values.size() != 1)
			return false;

		const StaticString* value;
		if (!values->get(&value))
			return false;

		if (*value == "normal"_s)
			*state = WidgetState::Normal;
		else if (*value == "active"_s)
			*state = WidgetState::Active;
		else if (*value == "pressed"_s)
			*state = WidgetState::Pressed;
		else if (*value == "checked"_s)
			*state = WidgetState::Checked;
		else if (*value == "disabled"_s)
			*state = WidgetState::Disabled;
		else
			return false;

		return true;
	}

	bool GuiIonPropertyLoader::load_text(const StaticString** text, const IonNode& node)
	{
		return node.size() == 1 && node.first()->get(text);
	}

	bool GuiIonPropertyLoader::load_texture(std::shared_ptr<const Texture2D>& texture, Texture2D::Filter& filter, const IonNode& node, ResourceLoader& resource_loader)
	{
		auto&& values = node.values();

		if (values.is_empty() || values.size() > 4)
			return false;

		const StaticString* texture_name;
		if (!values->get(&texture_name))
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
				const StaticString* value;
				if (!values->get(&value))
					return false;

				if (*value == "nearest"_s)
				{
					if (has_interpolation)
						return false;
					filter = static_cast<Texture2D::Filter>(Texture2D::NearestFilter | (filter & Texture2D::AnisotropicFilter));
					has_interpolation = true;
				}
				else if (*value == "linear"_s)
				{
					if (has_interpolation)
						return false;
					filter = static_cast<Texture2D::Filter>(Texture2D::LinearFilter | (filter & Texture2D::AnisotropicFilter));
					has_interpolation = true;
				}
				else if (*value == "bilinear"_s)
				{
					if (has_interpolation)
						return false;
					filter = static_cast<Texture2D::Filter>(Texture2D::BilinearFilter | (filter & Texture2D::AnisotropicFilter));
					has_interpolation = true;
				}
				else if (*value == "trilinear"_s)
				{
					if (has_interpolation)
						return false;
					filter = static_cast<Texture2D::Filter>(Texture2D::TrilinearFilter | (filter & Texture2D::AnisotropicFilter));
					has_interpolation = true;
				}
				else if (*value == "anisotropic"_s)
				{
					if (!has_interpolation || has_anisotropy)
						return false;
					filter = static_cast<Texture2D::Filter>((filter & Texture2D::IsotropicFilterMask) | Texture2D::AnisotropicFilter);
					has_anisotropy = true;
				}
				else
				{
					return false;
				}
			}
		}

		const auto result_texture = resource_loader.load_texture_2d(*texture_name);
		if (!result_texture)
			return false;

		texture = result_texture;

		return true;
	}
}
