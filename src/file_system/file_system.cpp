#include <Yttrium/file_system.hpp>

#include <atomic> // std::atomic

#include "../base/instance_guard.hpp"

namespace Yttrium
{

typedef InstanceGuard<FileSystem> FileSystemGuard;

FileSystem::FileSystem(Allocator *allocator)
	: _allocator(allocator)
	, _order(PackedFirst)
{
	FileSystemGuard::enter(this, Y_S("Duplicate FileSystem construction"));
}

FileSystem::~FileSystem()
{
	unmount_all();

	FileSystemGuard::leave(this, Y_S("Unmatched FileSystem destruction"));
}

File FileSystem::open_file(const StaticString &name, File::Mode mode, Order order)
{
	if (mode != File::Read)
	{
		return File(name, mode, _allocator);
	}

	File file;

	if (order == PresetOrder)
	{
		order = _order;
	}

	if (order == PackedFirst || order == PackedOnly)
	{
		file = open_packed(name);
		if (file.is_opened())
		{
			return file;
		}
	}

	if (order != PackedOnly)
	{
		if (file.open(name, mode, _allocator))
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

bool FileSystem::mount(const StaticString &name, PackageFormat format)
{
	PackageReader *package = PackageReader::open(name, format);

	if (package)
	{
		_packages.push_back(package);
	}

	return package;
}

void FileSystem::unmount_all()
{
	for (Packages::reverse_iterator i = _packages.rbegin(); i != _packages.rend(); ++i)
	{
		delete *i;
	}

	_packages.clear();
}

FileSystem *FileSystem::instance()
{
	return FileSystemGuard::instance;
}

File FileSystem::open_packed(const StaticString &name) const
{
	File file;

	for (Packages::const_reverse_iterator i = _packages.rbegin(); i != _packages.rend(); ++i)
	{
		file = (*i)->open_file(name, _allocator);

		if (file.is_opened())
		{
			break;
		}
	}
	return file;
}

} // namespace Yttrium
