// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "dds.h"

#include <yttrium/image/image.h>
#include <yttrium/storage/reader.h>
#include "../formats.h"

namespace Yt
{
	bool read_dds_header(Reader& reader, ImageInfo& info)
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
			return false;

		PixelFormat pixel_format;
		switch (header.ddspf.dwFlags & (DDPF_ALPHAPIXELS | DDPF_RGB | DDPF_LUMINANCE))
		{
		case DDPF_RGB:
			if (header.ddspf.dwRGBBitCount != 24)
				return false;
			if (header.ddspf.dwRBitMask == 0xff0000 && header.ddspf.dwGBitMask == 0xff00 && header.ddspf.dwBBitMask == 0xff && !header.ddspf.dwABitMask)
				pixel_format = PixelFormat::Bgr24;
			else if (header.ddspf.dwRBitMask == 0xff && header.ddspf.dwGBitMask == 0xff00 && header.ddspf.dwBBitMask == 0xff0000 && !header.ddspf.dwABitMask)
				pixel_format = PixelFormat::Rgb24;
			else
				return false;
			break;

		case DDPF_RGB | DDPF_ALPHAPIXELS:
			if (header.ddspf.dwRGBBitCount != 32)
				return false;
			if (header.ddspf.dwRBitMask == 0xff0000 && header.ddspf.dwGBitMask == 0xff00 && header.ddspf.dwBBitMask == 0xff && header.ddspf.dwABitMask == 0xff000000)
				pixel_format = PixelFormat::Bgra32;
			else if (header.ddspf.dwRBitMask == 0xff && header.ddspf.dwGBitMask == 0xff00 && header.ddspf.dwBBitMask == 0xff0000 && header.ddspf.dwABitMask == 0xff000000)
				pixel_format = PixelFormat::Rgba32;
			else
				return false;
			break;

		case DDPF_LUMINANCE:
			if (header.ddspf.dwRGBBitCount != 8)
				return false;
			if (header.ddspf.dwRBitMask == 0xff && !header.ddspf.dwGBitMask && !header.ddspf.dwBBitMask && !header.ddspf.dwABitMask)
				pixel_format = PixelFormat::Gray8;
			else
				return false;
			break;

		case DDPF_LUMINANCE | DDPF_ALPHAPIXELS:
			if (header.ddspf.dwRGBBitCount != 16)
				return false;
			if (header.ddspf.dwRBitMask == 0xff && !header.ddspf.dwGBitMask && !header.ddspf.dwBBitMask && header.ddspf.dwABitMask == 0xff00)
				pixel_format = PixelFormat::GrayAlpha16;
			else
				return false;
			break;

		default:
			return false;
		}

		info = { header.dwWidth, header.dwHeight, pixel_format };
		return true;
	}
}
