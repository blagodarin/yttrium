#include <yttrium/exceptions.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/memory/unique_ptr.h>
#include <yttrium/resources/resource_ptr.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>
#include <yttrium/string.h>

#include <iostream>
#include <vector>

using namespace Yttrium;

template <typename... Args>
void check(bool condition, Args&&... args)
{
	if (!condition)
		throw DataError(std::forward<Args>(args)...);
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ypq PACKAGE INDEX" << std::endl;
		return 1;
	}

	std::vector<std::pair<String, std::vector<std::pair<String, String>>>> entries;
	try
	{
		const auto document = IonDocument::open(Reader(argv[2]));
		check(!!document, "Bad index file");

		const auto& root_object = document->root();
		check(document->root().size() == 1, "Index file must have a single 'package' node");

		const auto& package_node = *root_object.begin();
		check(package_node.name() == "package", "Index file must have a single 'package' node");
		check(package_node.size() == 1, "Bad 'package' node");

		const auto& list_value = *package_node.begin();
		check(list_value.type() == IonValue::Type::List, "Bad 'package' node");

		for (const auto& entry : list_value.list())
		{
			switch (entry.type())
			{
			case IonValue::Type::String:
				entries.emplace_back(String(entry.string()), std::vector<std::pair<String, String>>());
				break;
			case IonValue::Type::Object:
				check(!entries.empty(), "Properties must follow a file");
				check(entries.back().second.empty(), "\"", entries.back().first, "\": Multiple property sets are not allowed");
				{
					const auto& properties = *entry.object();
					check(properties.size() > 0, "\"", entries.back().first, "\": Property set must not be empty");
					for (const auto& property : properties)
					{
						check(property.size() == 1, "\"", entries.back().first, "\": Bad property '", property.name(), "'");
						check(property.begin()->type() == IonValue::Type::String, "\"", entries.back().first, "\": Bad property '", property.name(), "' value");
						entries.back().second.emplace_back(String(property.name()), String(property.begin()->string()));
					}
				}
				break;
			default:
				check(false, "Bad 'package' entry");
			}
		}
	}
	catch (const DataError& e)
	{
		std::cerr << "ERROR(" << argv[2] << "): " << e.what() << std::endl;
		return 1;
	}

	const auto package = PackageWriter::create(argv[1], PackageType::Ypq);
	if (!package)
	{
		std::cerr << "ERROR(" << argv[1] << "): Can't open package file" << std::endl;
		return 1;
	}

	for (const auto& entry : entries)
	{
		if (!entry.second.empty())
			std::cerr << "WARNING(" << argv[2] << "): \"" << entry.first << "\": Properties are not yet supported" << std::endl;
		if (!package->add(entry.first, Reader(entry.first)))
		{
			package->unlink();
			std::cerr << "ERROR(" << argv[2] << "): \"" << entry.first << "\": Can't pack" << std::endl;
			return 1;
		}
	}
}
