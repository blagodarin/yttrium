#include "package.h"

#include "ypq.h"

namespace Yttrium
{

Y_IMPLEMENT_PRIVATE(PackageReader);

PackageReader::PackageReader(const StaticString& name, PackageType type, Allocator* allocator)
	: PackageReader()
{
	if (type == PackageType::Auto)
	{
		const StaticString extension = name.file_extension();
		if (extension == S(".ypq"))
			type = PackageType::Ypq;
		else
			return;
	}

	PrivateHolder<PackageReader::Private> reader;

	switch (type)
	{
	case PackageType::Ypq: reader.reset<YpqReader>(allocator, name); break;
	default: return;
	}

	if (reader->_file && reader->open())
		_private = reader.release();
}

File PackageReader::open_file(const StaticString& name)
{
	if (_private)
	{
		const PackedFile& packed_file = _private->open_file(name);
		if (packed_file.file)
			return File(packed_file.file->_private, packed_file.file->_offset, packed_file.size);
	}
	return File();
}

Y_IMPLEMENT_PRIVATE(PackageWriter);

bool PackageWriter::Private::open(PackageWriter::Mode mode)
{
	if (mode == PackageWriter::Append)
		_file.seek(0, File::Reverse);
	return true;
}

PackageWriter::PackageWriter(const StaticString& name, PackageType type, Mode mode, Allocator* allocator)
{
	if (type == PackageType::Auto)
	{
		const StaticString extension = name.file_extension();
		if (extension == S(".ypq"))
			type = PackageType::Ypq;
		else
			return;
	}

	PrivateHolder<PackageWriter::Private> writer;

	const unsigned file_mode = File::Write | (mode & Append ? 0 : File::Truncate);
	switch (type)
	{
	case PackageType::Ypq: writer.reset<YpqWriter>(allocator, name, file_mode); break;
	default: return;
	}

	if (writer->_file && writer->open(mode))
		_private = writer.release();
}

File PackageWriter::open_file(const StaticString& name)
{
	if (_private)
	{
		const PackedFile &packed_file = _private->open_file(name);
		if (packed_file.file)
			return File(packed_file.file->_private, packed_file.file->_offset, packed_file.size);
	}
	return File();
}

} // namespace Yttrium
