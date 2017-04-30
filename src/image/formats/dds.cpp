#include "dds.h"
#include "../formats.h"

#include <yttrium/image.h>
#include <yttrium/storage/reader.h>

namespace Yttrium
{
	std::optional<ImageFormat> read_dds_header(Reader& reader)
	{
		DDS_HEADER header;
		if (!reader.read(header)
			|| header.dwMagic != DDS_HEADER::MAGIC
			|| header.dwSize != DDS_HEADER::SIZE
			|| !(header.dwFlags & (DDSD_HEIGHT | DDSD_WIDTH))
			|| (header.dwFlags & (DDSD_LINEARSIZE | DDSD_DEPTH))
			|| !header.dwHeight
			|| !header.dwWidth
			|| header.dwDepth
			|| header.dwMipMapCount
			|| header.dwReserved1[0] || header.dwReserved1[1] || header.dwReserved1[2] || header.dwReserved1[3]
			|| header.dwReserved1[4] || header.dwReserved1[5] || header.dwReserved1[6] || header.dwReserved1[7]
			|| header.dwReserved1[8] || header.dwReserved1[9] || header.dwReserved1[10]
			|| header.ddspf.dwSize != DDS_PIXELFORMAT::SIZE
			|| (header.ddspf.dwFlags & (DDPF_ALPHA | DDPF_FOURCC | DDPF_YUV))
			|| (header.dwCaps & (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP))
			|| header.dwCaps2
			|| header.dwCaps3
			|| header.dwCaps4
			|| header.dwReserved2)
			return {};

		std::optional<ImageFormat> format;

		switch (header.ddspf.dwFlags & (DDPF_ALPHAPIXELS | DDPF_RGB | DDPF_LUMINANCE))
		{
		case DDPF_RGB:
			if (header.ddspf.dwRGBBitCount != 24)
				return {};
			if (header.ddspf.dwRBitMask == 0xff0000 && header.ddspf.dwGBitMask == 0xff00 && header.ddspf.dwBBitMask == 0xff && !header.ddspf.dwABitMask)
				format.emplace(header.dwWidth, header.dwHeight, PixelFormat::Bgr, 24);
			else if (header.ddspf.dwRBitMask == 0xff && header.ddspf.dwGBitMask == 0xff00 && header.ddspf.dwBBitMask == 0xff0000 && !header.ddspf.dwABitMask)
				format.emplace(header.dwWidth, header.dwHeight, PixelFormat::Rgb, 24);
			else
				return {};
			break;

		case DDPF_RGB | DDPF_ALPHAPIXELS:
			if (header.ddspf.dwRGBBitCount != 32)
				return {};
			if (header.ddspf.dwRBitMask == 0xff0000 && header.ddspf.dwGBitMask == 0xff00 && header.ddspf.dwBBitMask == 0xff && header.ddspf.dwABitMask == 0xff000000)
				format.emplace(header.dwWidth, header.dwHeight, PixelFormat::Bgra, 32);
			else if (header.ddspf.dwRBitMask == 0xff && header.ddspf.dwGBitMask == 0xff00 && header.ddspf.dwBBitMask == 0xff0000 && header.ddspf.dwABitMask == 0xff000000)
				format.emplace(header.dwWidth, header.dwHeight, PixelFormat::Rgba, 32);
			else
				return {};
			break;

		case DDPF_LUMINANCE:
			if (header.ddspf.dwRGBBitCount != 8)
				return {};
			if (header.ddspf.dwRBitMask == 0xff && !header.ddspf.dwGBitMask && !header.ddspf.dwBBitMask && !header.ddspf.dwABitMask)
				format.emplace(header.dwWidth, header.dwHeight, PixelFormat::Gray, 8);
			else
				return {};
			break;

		case DDPF_LUMINANCE | DDPF_ALPHAPIXELS:
			if (header.ddspf.dwRGBBitCount != 16)
				return {};
			if (header.ddspf.dwRBitMask == 0xff && !header.ddspf.dwGBitMask && !header.ddspf.dwBBitMask && header.ddspf.dwABitMask == 0xff00)
				format.emplace(header.dwWidth, header.dwHeight, PixelFormat::GrayAlpha, 16);
			else
				return {};
			break;

		default:
			return {};
		}

		return format;
	}
}
