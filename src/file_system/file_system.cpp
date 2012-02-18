#include "file_system.hpp"

namespace Yttrium
{

File FileSystem::open_file(const StaticString &name, File::Mode mode, Order order) noexcept
{
	if (mode != File::Read)
	{
		return File(name, mode); // NOTE: No allocator specified!
	}

	File file;

	if (order == PresetOrder)
	{
		order = _order;
	}

	if (order == PackedFirst || order == PackedOnly)
	{
		if ((file = open_packed(name)))
		{
			return file;
		}
	}

	if (order != PackedOnly)
	{
		if ((file = File(name, mode)))
		{
			return file;
		}

		if (order != SystemOnly)
		{
			return open_packed(name);
		}
	}

	return file;
}

bool FileSystem::mount(const StaticString &name, PackageFormat format) noexcept
{
	PackageReader *package = PackageReader::open(name, format);

	if (package)
	{
		_packages.push_back(package);
	}

	return package;
}

void FileSystem::unmount_all() noexcept
{
	for (Packages::reverse_iterator i = _packages.rbegin(); i != _packages.rend(); ++i)
	{
		delete *i;
	}

	_packages.clear();
}

FileSystem &FileSystem::instance() noexcept
{
	return *_file_system;
}

File FileSystem::open_packed(const StaticString &name) const
{
	File file;

	for (Packages::const_reverse_iterator i = _packages.rbegin(); i != _packages.rend(); ++i)
	{
		file = (*i)->open_file(name);
		if (file)
		{
			break;
		}
	}
	return file;
}

FileSystem *_file_system = nullptr;

} // namespace Yttrium
