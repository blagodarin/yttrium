#include "package.h"

#include "ypq.h"

namespace Yttrium
{

PackageReader::PackageReader(const PackageReader &reader)
	: _private(Private::copy(reader._private))
{
}

void PackageReader::close()
{
	Private::release(&_private);
}

bool PackageReader::open(const StaticString &name, PackageType type, Allocator *allocator)
{
	close();

	if (Y_LIKELY(type == PackageType::Auto))
	{
		StaticString extension = name.file_extension();
		if (extension == ".ypq")
			type = PackageType::Ypq;
		else
			return false;
	}

	switch (type)
	{
	case PackageType::Ypq: _private = Y_NEW(allocator, YpqReader)(allocator); break;
	default:               return false;
	}

	if (Y_LIKELY(_private->_file.open(name, allocator) && _private->open()))
		return true;

	close();
	return false;
}

File PackageReader::open_file(const StaticString &name)
{
	if (Y_LIKELY(_private))
	{
		const PackedFile &packed_file = _private->open_file(name);
		if (Y_LIKELY(packed_file.file))
			return File(packed_file.file->_private, packed_file.file->_offset, packed_file.size);
	}
	return File();
}

PackageReader &PackageReader::operator =(const PackageReader &reader)
{
	Private::assign(&_private, reader._private);
	return *this;
}

PackageWriter::PackageWriter(const PackageWriter &reader)
	: _private(Private::copy(reader._private))
{
}

void PackageWriter::close()
{
	Private::release(&_private);
}

bool PackageWriter::open(const StaticString &name, PackageType type, Mode mode, Allocator *allocator)
{
	close();

	if (Y_LIKELY(type == PackageType::Auto))
	{
		StaticString extension = name.file_extension();
		if (extension == ".ypq")
			type = PackageType::Ypq;
		else
			return false;
	}

	switch (type)
	{
	case PackageType::Ypq: _private = Y_NEW(allocator, YpqWriter)(allocator); break;
	default:               return false;
	}

	if (Y_LIKELY(_private->_file.open(name, File::Write | (mode & Append ? 0 : File::Truncate)) && _private->open(mode)))
		return true;

	close();
	return false;
}

File PackageWriter::open_file(const StaticString &name)
{
	if (Y_LIKELY(_private))
	{
		const PackedFile &packed_file = _private->open_file(name);
		if (Y_LIKELY(packed_file.file))
			return File(packed_file.file->_private, packed_file.file->_offset, packed_file.size);
	}
	return File();
}

PackageWriter &PackageWriter::operator =(const PackageWriter &writer)
{
	Private::assign(&_private, writer._private);
	return *this;
}

bool PackageWriter::Private::open(PackageWriter::Mode mode)
{
	if (mode == PackageWriter::Append)
		_file.seek(0, File::Reverse);
	return true;
}

} // namespace Yttrium
