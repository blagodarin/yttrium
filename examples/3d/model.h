#ifndef _examples_3d_model_h_
#define _examples_3d_model_h_

#include <yttrium/memory/unique_ptr.h>
#include <yttrium/resource/resource_ptr.h>

namespace Yttrium
{
	class GpuProgram;
	class IndexBuffer;
	class Renderer;
	class Texture2D;
	class Vector4;
	class VertexBuffer;
}

using namespace Yttrium;

class Model
{
public:
	Model(Renderer&);
	~Model();

	void draw(const Vector4& translation);

protected:
	Renderer& _renderer;
	UniquePtr<VertexBuffer> _vertices;
	UniquePtr<IndexBuffer> _indices;
	ResourcePtr<Texture2D> _texture;
	UniquePtr<GpuProgram> _program;
};

class CubeModel : public Model
{
public:
	CubeModel(Renderer&);
};

class ChessboardModel : public Model
{
public:
	ChessboardModel(Renderer&);
};

#endif
