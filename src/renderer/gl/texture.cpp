#include "texture.h"

namespace Yttrium
{

OpenGlTexture2D::~OpenGlTexture2D()
{
	_gl.DeleteTextures(1, &_texture);
}

} // namespace Yttrium
