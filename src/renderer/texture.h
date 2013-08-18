#ifndef __RENDERER_TEXTURE_H
#define __RENDERER_TEXTURE_H

#include <Yttrium/image.h>
#include <Yttrium/renderer.h>
#include <Yttrium/renderer/texture.h>

namespace Yttrium
{

class BackendTexture2D: public Texture2D
{
public:

	BackendTexture2D(const Renderer &renderer, const ImageFormat &format, Allocator *allocator);

public:
	
	RectF full_rectangle() const;

	Dim2 size() const { return _size; }

public:

	virtual void bind() = 0;

	virtual Vector2f fix_coords(const Vector2f &coords) const = 0; // TODO: fix_rectangle().

	virtual void unbind() = 0;

protected:

	const Dim2             _size;
	const ImageOrientation _orientation;
	bool                   _has_mipmaps;

private:

	Renderer _renderer; // Don't let the renderer die before the texture.
};

} // namespace Yttrium

#endif // __RENDERER_TEXTURE_H
