#ifndef _examples_3d_model_h_
#define _examples_3d_model_h_

#include "material.h"

namespace Yttrium
{
	class Mesh;
	class Vector4;
}

class Model
{
public:
	Model(Renderer&, ResourceLoader&, const StaticString& mesh, const StaticString& material);
	~Model();

	void draw();

protected:
	Renderer& _renderer;
	ResourcePtr<const Mesh> _mesh;
	Material _material; // TODO: Make 'const'.
};

#endif
