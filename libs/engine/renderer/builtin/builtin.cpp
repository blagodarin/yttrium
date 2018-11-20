#include "builtin.h"

#include <yttrium/exceptions.h>
#include <yttrium/image.h>
#include <yttrium/renderer/program.h>
#include "../backend.h"
#include "debug_texture.h"

namespace
{
	const int32_t _white_texture_data = -1;
}

namespace Yttrium
{
	RenderBuiltin::RenderBuiltin(RenderBackend& backend)
		: _white_texture{ backend.create_texture_2d({ { 1, 1, PixelFormat::Bgra32 }, &_white_texture_data }, RenderManager::TextureFlag::NoMipmaps) }
		, _debug_texture{ backend.create_texture_2d({ { DebugTexture::width, DebugTexture::height, PixelFormat::Bgra32 }, DebugTexture::data }, RenderManager::TextureFlag::NoMipmaps) }
		, _program_2d{ backend.create_builtin_program_2d() }
	{
		if (!_white_texture || !_debug_texture)
			throw InitializationError("Failed to initialize an internal texture");
		if (!_program_2d)
			throw InitializationError("Failed to initialize an internal rendering program");
	}

	RenderBuiltin::~RenderBuiltin() noexcept = default;
}
