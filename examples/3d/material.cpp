#include "material.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/storage.h>

#include <stdexcept>

Material::Material(ResourceLoader& resource_loader, const StaticString& name)
{
	StaticString vertex_shader;
	StaticString fragment_shader;
	for (const auto& node : resource_loader.load_ion(name)->root())
	{
		if (node.name() == "vertex_shader")
		{
			if (node.size() != 1 || node.begin()->type() != IonValue::Type::String || node.begin()->string().is_empty())
				throw std::runtime_error("Bad 'vertex_shader'");
			if (!vertex_shader.is_empty())
				throw std::runtime_error("Duplicate 'vertex_shader'");
			vertex_shader = node.begin()->string();
		}
		else if (node.name() == "fragment_shader")
		{
			if (node.size() != 1 || node.begin()->type() != IonValue::Type::String || node.begin()->string().is_empty())
				throw std::runtime_error("Bad 'fragment_shader'");
			if (!fragment_shader.is_empty())
				throw std::runtime_error("Duplicate 'fragment_shader'");
			fragment_shader = node.begin()->string();
		}
		else if (node.name() == "texture")
		{
			if (node.size() != 1 || node.begin()->type() != IonValue::Type::String || node.begin()->string().is_empty())
				throw std::runtime_error("Bad 'texture'");
			if (_texture)
				throw std::runtime_error("Duplicate 'texture'");
			_texture = resource_loader.load_texture_2d(node.begin()->string());
		}
		else
			throw std::runtime_error("Bad material");
	}
	if (vertex_shader.is_empty() || fragment_shader.is_empty())
		throw std::runtime_error("No 'vertex_shader' or 'fragment_shader'");
	_program = resource_loader.renderer()->create_gpu_program(resource_loader.storage().open(vertex_shader).to_string(), resource_loader.storage().open(fragment_shader).to_string());
	if (!_program)
		throw std::runtime_error("Bad 'vertex_shader' or 'fragment_shader'");
}

Material::~Material() = default;
