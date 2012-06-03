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
	if (Private::should_free(&_private))
	{
		Private::free(&_private);
	}
}

bool PackageReader::open(const StaticString &name, PackageType type, Allocator *allocator)
{
	close();

	switch (type)
	{
	case PackageType::Ypq:

		_private = new(allocator->allocate<YpqReader>()) YpqReader(allocator);
		break;

	default:

		{
			StaticString extension = name.file_extension();

			if (extension == ".ypq")
			{
				_private = new(allocator->allocate<YpqReader>()) YpqReader(allocator);
			}
		}
		break;
	}

	if (_private)
	{
		if (_private->_file.open(name, allocator)
			&& _private->open())
		{
			return true;
		}

		close();
	}

	return false;
}

File PackageReader::open_file(const StaticString &name)
{
	if (_private)
	{
		const PackedFile &packed_file = _private->open_file(name);

		if (packed_file.file)
		{
			File(packed_file.file->_private, packed_file.file->_offset, packed_file.size);
		}
	}

	return File();
}

PackageReader &PackageReader::operator =(const PackageReader &reader)
{
	close();

	_private = Private::copy(reader._private);

	return *this;
}

PackageWriter::PackageWriter(const PackageWriter &reader)
	: _private(Private::copy(reader._private))
{
}

void PackageWriter::close()
{
	if (Private::should_free(&_private))
	{
		Private::free(&_private);
	}
}

bool PackageWriter::open(const StaticString &name, PackageType type, Allocator *allocator)
{
	close();

	switch (type)
	{
	case PackageType::Ypq:

		_private = new(allocator->allocate<YpqWriter>()) YpqWriter(allocator);
		break;

	default:

		{
			StaticString extension = name.file_extension();

			if (extension == ".ypq")
			{
				_private = new(allocator->allocate<YpqWriter>()) YpqWriter(allocator);
			}
		}
		break;
	}

	if (_private)
	{
		if (_private->_file.open(name, File::Write)
			&& _private->open())
		{
			return true;
		}

		close();
	}

	return false;
}

File PackageWriter::open_file(const StaticString &name)
{
	if (_private)
	{
		const PackedFile &packed_file = _private->open_file(name);

		if (packed_file.file)
		{
			File(packed_file.file->_private, packed_file.file->_offset, packed_file.size);
		}
	}

	return File();
}

PackageWriter &PackageWriter::operator =(const PackageWriter &writer)
{
	close();

	_private = Private::copy(writer._private);

	return *this;
}

PackageReader::Private::~Private()
{
}

PackageWriter::Private::~Private()
{
}

bool PackageWriter::Private::open()
{
	return true;
}

} // namespace Yttrium
