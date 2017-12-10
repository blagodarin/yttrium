#ifndef _examples_3d_model_h_
#define _examples_3d_model_h_

#include <memory>
#include <string_view>

namespace Yttrium
{
	class Material;
	class Mesh;
	class RenderContext;
	class ResourceLoader;
}

using namespace Yttrium;

class Model
{
public:
	Model(ResourceLoader&, std::string_view mesh, std::string_view material);
	~Model();

	void draw(RenderContext&);

private:
	const std::shared_ptr<const Mesh> _mesh;
	const std::shared_ptr<const Material> _material;
};

#endif
