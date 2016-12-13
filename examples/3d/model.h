#ifndef _examples_3d_model_h_
#define _examples_3d_model_h_

#include <yttrium/resources/resource_ptr.h>

namespace Yttrium
{
	class Material;
	class Mesh;
	class Renderer;
	class ResourceLoader;
	class StaticString;
}

using namespace Yttrium;

class Model
{
public:
	Model(Renderer&, ResourceLoader&, const StaticString& mesh, const StaticString& material);
	~Model();

	void draw();

protected:
	Renderer& _renderer;
	const ResourcePtr<const Mesh> _mesh;
	const ResourcePtr<const Material> _material;
};

#endif
