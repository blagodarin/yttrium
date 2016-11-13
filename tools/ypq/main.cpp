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
#include <limits>
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

	std::vector<std::pair<String, std::map<std::string, std::string>>> entries;
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
				entries.emplace_back(String(entry.string()), std::map<std::string, std::string>());
				break;
			case IonValue::Type::Object:
				check(!entries.empty(), "Property set must follow a file");
				check(entries.back().second.empty(), "\"", entries.back().first, "\": Multiple property sets are not allowed");
				{
					const auto& properties = *entry.object();
					check(properties.size() > 0, "\"", entries.back().first, "\": Property set must not be empty");
					for (const auto& property : properties)
					{
						check(property.size() == 1, "\"", entries.back().first, "\": Bad property '", property.name(), "'");
						check(property.begin()->type() == IonValue::Type::String, "\"", entries.back().first, "\": Bad property '", property.name(), "' value");
						std::string property_name(property.name().text(), property.name().size());
						check(entries.back().second.count(property_name) == 0, "\": Duplicate property '", property.name(), "'");
						std::string property_value(property.begin()->string().text(), property.begin()->string().size());
						entries.back().second.emplace(std::move(property_name), std::move(property_value));
						// TODO: Usa actual limits from the implementation.
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

	for (auto& entry : entries)
	{
		if (!package->add(entry.first, Reader(entry.first), std::move(entry.second)))
		{
			std::cerr << "ERROR(" << argv[2] << "): \"" << entry.first << "\": Can't pack" << std::endl;
			return 1;
		}
	}

	package->commit();
}
