#include "dds.h"

#include "dds_format.h"

namespace Yttrium
{
	DdsReader::DdsReader(const StaticString& name, Allocator& allocator)
		: ImageReader(name, allocator)
	{
	}

	bool DdsReader::open()
	{
		DDS_HEADER header;

		if (!_file.read(&header)
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
		{
			return false;
		}

		switch (header.ddspf.dwFlags & (DDPF_ALPHAPIXELS | DDPF_RGB | DDPF_LUMINANCE))
		{
		case DDPF_RGB:

			if (header.ddspf.dwRGBBitCount != 24)
				return false;

			if (header.ddspf.dwRBitMask == 0x00FF0000
				&& header.ddspf.dwGBitMask == 0x0000FF00
				&& header.ddspf.dwBBitMask == 0x000000FF
				&& !header.ddspf.dwABitMask)
			{
				_format.set_pixel_format(PixelFormat::Bgr, 24);
			}
			else if (header.ddspf.dwRBitMask == 0x000000FF
				&& header.ddspf.dwGBitMask == 0x0000FF00
				&& header.ddspf.dwBBitMask == 0x00FF0000
				&& !header.ddspf.dwABitMask)
			{
				_format.set_pixel_format(PixelFormat::Rgb, 24);
			}
			else
			{
				return false;
			}
			break;

		case DDPF_RGB | DDPF_ALPHAPIXELS:

			if (header.ddspf.dwRGBBitCount != 32)
				return false;

			if (header.ddspf.dwRBitMask == 0x00FF0000
				&& header.ddspf.dwGBitMask == 0x0000FF00
				&& header.ddspf.dwBBitMask == 0x000000FF
				&& header.ddspf.dwABitMask == 0xFF000000)
			{
				_format.set_pixel_format(PixelFormat::Bgra, 32);
			}
			else if (header.ddspf.dwRBitMask == 0x000000FF
				&& header.ddspf.dwGBitMask == 0x0000FF00
				&& header.ddspf.dwBBitMask == 0x00FF0000
				&& header.ddspf.dwABitMask == 0xFF000000)
			{
				_format.set_pixel_format(PixelFormat::Rgba, 32);
			}
			else
			{
				// We could also load ARGB/ABGR data, but that's rarely needed.
				// Also, is there a DDS with such data layout?
				return false;
			}
			break;

		case DDPF_LUMINANCE:

			if (header.ddspf.dwRGBBitCount != 8)
				return false;

			if (header.ddspf.dwRBitMask == 0x000000FF
				&& !header.ddspf.dwGBitMask
				&& !header.ddspf.dwBBitMask
				&& !header.ddspf.dwABitMask)
			{
				_format.set_pixel_format(PixelFormat::Gray, 8);
			}
			else
			{
				return false;
			}
			break;

		case DDPF_LUMINANCE | DDPF_ALPHAPIXELS:

			if (header.ddspf.dwRGBBitCount != 16)
				return false;

			if (header.ddspf.dwRBitMask == 0x000000FF
				&& !header.ddspf.dwGBitMask
				&& !header.ddspf.dwBBitMask
				&& header.ddspf.dwABitMask == 0x0000FF00)
			{
				_format.set_pixel_format(PixelFormat::GrayAlpha, 16);
			}
			else
			{
				return false;
			}
			break;

		default:

			return false;
		}

		_format.set_orientation(ImageOrientation::XRightYDown);
		_format.set_width(header.dwWidth);
		_format.set_height(header.dwHeight);

		return true;
	}

	bool DdsReader::read(void* buffer)
	{
		size_t frame_size = _format.frame_size();
		return _file.read(buffer, frame_size) == frame_size;
	}
}
