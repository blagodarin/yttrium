#include "material.h"

#include <yttrium/exceptions.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/storage.h>

namespace Yttrium
{
	MaterialImpl::MaterialImpl(ResourceLoader& resource_loader, const StaticString& name)
	{
		StaticString vertex_shader;
		StaticString fragment_shader;
		for (const auto& node : resource_loader.load_ion(name)->root())
		{
			if (node.name() == "vertex_shader"_s)
			{
				if (node.size() != 1 || node.begin()->type() != IonValue::Type::String || node.begin()->string().is_empty())
					throw DataError("("_s, name, ") Bad 'vertex_shader'"_s);
				if (!vertex_shader.is_empty())
					throw DataError("("_s, name, ") Duplicate 'vertex_shader'"_s);
				vertex_shader = node.begin()->string();
			}
			else if (node.name() == "fragment_shader"_s)
			{
				if (node.size() != 1 || node.begin()->type() != IonValue::Type::String || node.begin()->string().is_empty())
					throw DataError("("_s, name, ") Bad 'fragment_shader'"_s);
				if (!fragment_shader.is_empty())
					throw DataError("("_s, name, ") Duplicate 'fragment_shader'"_s);
				fragment_shader = node.begin()->string();
			}
			else if (node.name() == "texture"_s)
			{
				if (node.size() != 1 || node.begin()->type() != IonValue::Type::String || node.begin()->string().is_empty())
					throw DataError("("_s, name, ") Bad 'texture'"_s);
				if (_texture)
					throw DataError("("_s, name, ") Duplicate 'texture'"_s);
				_texture = resource_loader.load_texture_2d(node.begin()->string());
			}
			else
				throw DataError("("_s, name, ") Bad material"_s);
		}
		if (vertex_shader.is_empty() || fragment_shader.is_empty())
			throw DataError("("_s, name, ") No 'vertex_shader' or 'fragment_shader'"_s);
		_program = resource_loader.renderer()->create_gpu_program(
			StaticString{ resource_loader.storage().open(vertex_shader).to_string() },
			StaticString{ resource_loader.storage().open(fragment_shader).to_string() });
		if (!_program)
			throw DataError("("_s, name, ") Bad 'vertex_shader' or 'fragment_shader'"_s);
	}
}
