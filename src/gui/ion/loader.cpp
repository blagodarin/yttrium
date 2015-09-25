#include "loader.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/utils.h>
#include <yttrium/ion/value.h>
#include <yttrium/log.h>
#include <yttrium/texture_cache.h>
#include "../gui.h"
#include "../scene.h"
#include "property_loader.h"

#include <cassert>

namespace Yttrium
{
	namespace
	{
		bool load_object(const IonList& source, const IonObject** object, const StaticString** name, IonListRange* classes)
		{
			const IonObject* result_object = nullptr;
			const StaticString* result_name = nullptr;
			const IonList* result_classes = nullptr;

			for (const IonValue& value : source)
			{
				switch (value.type())
				{
				case IonValue::Type::List:
					if (result_classes)
						return false;
					for (const IonValue& class_name : value.list())
						if (class_name.type() != IonValue::Type::String)
							return false;
					result_classes = &value.list();
					break;

				case IonValue::Type::Object:
					if (result_object)
						return false;
					result_object = value.object();
					break;

				default:
					assert(value.type() == IonValue::Type::String);
					if (result_name)
						return false;
					value.get(&result_name);
					break;
				}
			}

			if (!result_object)
				return false;

			*object = result_object;
			*name = result_name;
			*classes = result_classes ? result_classes->values() : IonListRange();

			return true;
		}

		bool load_object(const IonList& source, const IonObject** object, const StaticString** name, const StaticString** class_name)
		{
			const IonObject* result_object;
			const StaticString* result_name;
			IonListRange result_classes;

			if (!load_object(source, &result_object, &result_name, &result_classes))
				return false;

			// TODO: Make use of multiple classes.

			if (result_classes.size() > 1)
				return false;

			const StaticString* result_class = nullptr;

			if (!result_classes.is_empty())
				result_classes->get(&result_class);

			*object = result_object;
			*name = result_name;
			*class_name = result_class;

			return true;
		}
	}

	GuiIonLoader::GuiIonLoader(GuiImpl& gui)
		: _gui(gui)
		, _classes(_gui.internal_allocator())
	{
	}

	bool GuiIonLoader::load(const StaticString& source_name, bool is_internal)
	{
		IonDocument document(_gui.internal_allocator());

		if (!document.load(source_name))
		{
			Log() << "[Gui] Can't load \""_s << source_name << "\"..."_s;
			return false;
		}

		const auto& document_root = document.root();

		if (!is_internal)
		{
			Vector2 size;
			if (GuiIonPropertyLoader::load_size(&size, document_root.last("size"_s)))
				_gui.set_size(size);

			Scaling scaling;
			if (GuiIonPropertyLoader::load_scaling(&scaling, document_root.last("scale"_s)))
				_gui.set_scaling(scaling);
		}

		load(document_root);

		return true;
	}

	void GuiIonLoader::load(const IonObject& source)
	{
		for (const auto& node : source)
		{
			if (node.name() == "include"_s)
			{
				const StaticString* include_path;
				if (Ion::get(node, include_path))
					load(*include_path, true);
			}
			else if (node.name() == "class"_s)
			{
				const IonObject* object;
				const StaticString* object_name;
				const StaticString* class_name;

				if (!load_object(node, &object, &object_name, &class_name))
				{
					continue;
				}

				if (!_classes.add(*object_name, *object, class_name))
					Log() << "[Gui] Can' load class \""_s << *object_name << "\""_s;
			}
			else if (node.name() == "scene"_s)
			{
				const IonObject* object;
				const StaticString* object_name;
				const StaticString* class_name;

				if (!load_object(node, &object, &object_name, &class_name))
				{
					continue;
				}

				if (class_name && *class_name != "root"_s)
				{
					Log() << "[Gui] Unknown scene \""_s << *object_name << "\" option \""_s << *class_name << "\" ignored"_s;
					class_name = nullptr;
				}

				auto scene = _gui.create_scene(*object_name);
				if (scene)
				{
					load_scene(*scene, *object);
					_gui.add_scene(std::move(scene), class_name != nullptr);
				}
			}
			else if (node.name() == "on_scene_change"_s)
			{
				const auto& s = node.values();

				if (s.size() != 2 || s->type() != IonValue::Type::List || s.last().type() != IonValue::Type::String)
				{
					Log() << "[Gui] Bad 'on_scene_change'"_s;
					continue;
				}

				const StaticString* from;
				const StaticString* to;
				const StaticString* action;

				const auto& t = s->list().values();

				if (t.size() != 2 || !t->get(&from) || !t.last().get(&to) || !s.last().get(&action))
				{
					Log() << "[Gui] Bad 'on_scene_change'"_s;
					continue;
				}

				_gui.set_scene_change_action(
					String(*from, _gui.internal_allocator()),
					String(*to, _gui.internal_allocator()),
					String(*action, _gui.internal_allocator()));
			}
			else if (node.name() == "font"_s)
			{
				const IonObject* object;
				const StaticString* object_name;
				const StaticString* class_name;

				if (!load_object(node, &object, &object_name, &class_name))
					continue;

				const StaticString* font_name;
				const StaticString* texture_name;

				if (!(GuiIonPropertyLoader::load_text(&font_name, *object, "file"_s)
					&& GuiIonPropertyLoader::load_text(&texture_name, *object, "texture"_s)))
				{
					continue;
				}

				_gui.set_font(*object_name, *font_name, *texture_name);
			}
		}
	}

	void GuiIonLoader::load_scene(GuiScene& scene, const IonObject& source) const
	{
		scene.reserve(source.size());

		for (const auto& node : source)
		{
			if (node.name() == "size"_s)
			{
				Vector2 size;
				if (GuiIonPropertyLoader::load_size(&size, node))
					scene.set_size(size);
			}
			else if (node.name() == "scale"_s)
			{
				Scaling scaling;
				if (GuiIonPropertyLoader::load_scaling(&scaling, node))
					scene.set_scaling(scaling);
			}
			if (node.name() == "transparent"_s)
			{
				scene.set_transparent(true);
			}
			else if (node.name() == "bind"_s)
			{
				const auto s = node.values();
				if (s.size() == 2 && s->type() == IonValue::Type::String && s.last().type() == IonValue::Type::String)
					scene.bind(s->string(), s.last().string());
			}
			else
			{
				const IonObject* object;
				const StaticString* object_name;
				const StaticString* class_name;

				if (!load_object(node, &object, &object_name, &class_name))
					continue;

				GuiIonPropertyLoader loader(object, (class_name ? _classes.find(*class_name) : nullptr), _gui);

				scene.load_widget(node.name(), (object_name ? *object_name : StaticString()), loader);
			}
		}
	}
}
