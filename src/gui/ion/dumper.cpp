#include "dumper.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/string_format.h>
#include "../gui.h"
#include "../layer.h"

namespace Yttrium
{
	void GuiIonDumper::dump(const StaticString& filename) const
	{
		IonDocument document(&_gui.allocator());

		if (_gui._has_size)
		{
			IonNode* size_node = document.root().append("size"_s);
			size_node->append(String(&_gui.allocator()) << _gui._size.width());
			size_node->append(String(&_gui.allocator()) << _gui._size.height());
		}

		IonNode* scale_node = document.root().append("scale"_s);

		switch (_gui._scaling)
		{
		case Scaling::Stretch: scale_node->append("stretch"_s); break;
		case Scaling::Min:     scale_node->append("min"_s);     break;
		case Scaling::Max:     scale_node->append("max"_s);     break;
		case Scaling::Fit:     scale_node->append("fit"_s);     break;
		}

		// TODO: Dump fonts.

		for (const auto& layer : _gui._layers)
		{
			IonNode* layer_node = document.root().append("layer"_s);

			layer_node->append(layer.first);

			if (layer.first == _gui._layer_stack.front()->name())
				layer_node->append_list()->append("root"_s);
			else if (layer.second->is_transparent())
				layer_node->append_list()->append("transparent"_s);

			dump_layer(*layer.second, layer_node);
		}

		document.save(filename);
	}

	void GuiIonDumper::dump_layer(const GuiLayer& layer, IonNode* layer_node) const
	{
		IonObject* layer_object = layer_node->append_object();

		IonNode* size_node = layer_object->append("size"_s);

		size_node->append(String(&_gui.allocator()) << layer._size.width());
		size_node->append(String(&_gui.allocator()) << layer._size.height());

		IonNode* scale_node = layer_object->append("scale"_s);

		switch (layer._scaling)
		{
		case Scaling::Stretch: scale_node->append("stretch"_s); break;
		case Scaling::Min:     scale_node->append("min"_s);     break;
		case Scaling::Max:     scale_node->append("max"_s);     break;
		case Scaling::Fit:     scale_node->append("fit"_s);     break;
		}

		for (const auto& binding : layer._bindings)
		{
			const auto bound_node = layer_object->append("bind"_s);
			bound_node->append(binding.first);
			bound_node->append(binding.second);
		}

		// TODO: Dump widgets.
	}
}
