#ifndef __EXAMPLES_RTS_MODEL_H
#define __EXAMPLES_RTS_MODEL_H

#include <yttrium/gpu_program.h>
#include <yttrium/renderer.h>
#include <yttrium/texture.h>

#include <memory>

using namespace Yttrium;

class Model
{
public:

	Model(Renderer&);

	void draw(const Vector4& translation);

protected:

	Renderer& _renderer;
	std::unique_ptr<VertexBuffer> _vertices;
	std::unique_ptr<IndexBuffer> _indices;
	Pointer<Texture2D> _texture;
	std::unique_ptr<GpuProgram> _program;
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
