#ifndef _examples_3d_model_h_
#define _examples_3d_model_h_

#include <yttrium/gpu_program.h>
#include <yttrium/renderer.h>
#include <yttrium/texture.h>

using namespace Yttrium;

class Model
{
public:
	Model(Renderer& renderer): _renderer(renderer) {}

	void draw(const Vector4& translation);

protected:
	Renderer& _renderer;
	UniquePtr<VertexBuffer> _vertices;
	UniquePtr<IndexBuffer> _indices;
	UniquePtr<Texture2D> _texture;
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
