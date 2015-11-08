#ifndef _examples_rts_model_h_
#define _examples_rts_model_h_

#include <yttrium/gpu_program.h>
#include <yttrium/pointer.h>
#include <yttrium/renderer.h>
#include <yttrium/texture.h>

using namespace Yttrium;

class Model
{
public:

	Model(Renderer&);

	void draw(const Vector4& translation);

protected:

	Renderer& _renderer;
	Pointer<VertexBuffer> _vertices;
	Pointer<IndexBuffer> _indices;
	SharedPtr<Texture2D> _texture;
	Pointer<GpuProgram> _program;
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
