#include "file_system.hpp"

namespace Yttrium
{

FileReaderPtr FileSystem::open_reader(const StaticString &name, Order order)
{
	FileReaderPtr reader;

	order = effective_order(order);
	if (order == PackedFirst || order == PackedOnly)
	{
		if ((reader = open_packed(name)))
		{
			return reader;
		}
	}
	if (order != PackedOnly)
	{
		if ((reader = FileReader::open(name)))
		{
			return reader;
		}
		if (order != SystemOnly)
		{
			return open_packed(name);
		}
	}
	return FileReaderPtr();
}

bool FileSystem::mount(const StaticString &name, PackageFormat format)
{
	PackageReader *package = PackageReader::open(name, format);
	if (!package)
	{
		return false;
	}
	_packages.push_back(package);
	return true;
}

void FileSystem::unmount_all() throw()
{
	for (Packages::reverse_iterator i = _packages.rbegin(); i != _packages.rend(); ++i)
	{
		delete *i;
	}
	_packages.clear();
}

FileReaderPtr FileSystem::open_packed(const StaticString &name)
{
	for (Packages::reverse_iterator i = _packages.rbegin(); i != _packages.rend(); ++i)
	{
		FileReaderPtr file = (*i)->open_reader(name);
		if (file)
		{
			return file;
		}
	}
	return FileReaderPtr();
}

FileSystem &FileSystem::instance() throw()
{
	return *_file_system;
}

FileSystem *_file_system = nullptr;

} // namespace Yttrium
