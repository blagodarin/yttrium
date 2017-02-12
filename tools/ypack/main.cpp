#include <yttrium/exceptions.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/resources/resource_ptr.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>

#include <iostream>
#include <limits>
#include <vector>

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

using namespace Yttrium;

template <typename... Args>
void check(bool condition, Args&&... args)
{
	if (!condition)
		throw DataError(std::forward<Args>(args)...);
}

int main(int argc, char** argv)
{
	std::string index_name;
	std::string package_name;
	{
		boost::program_options::options_description public_options("Options");
		public_options.add_options()
			("dependencies,d", boost::program_options::value<bool>()->zero_tokens());

		const auto show_usage = [&public_options]
		{
			std::cerr
				<< "Usage:\n"
				<< "  ypack INDEX PACKAGE\n"
				<< "  ypack (--dependencies|-d) INDEX\n"
				<< "\n"
				<< public_options << "\n";
		};

		boost::program_options::options_description o;
		o.add(public_options).add_options()
			("input,i", boost::program_options::value<std::string>()->required())
			("output,o", boost::program_options::value<std::string>());

		boost::program_options::positional_options_description p;
		p.add("input", 1).add("output", 1);

		try
		{
			boost::program_options::variables_map vm;
			boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(o).positional(p).run(), vm);
			boost::program_options::notify(vm);
			index_name = vm["input"].as<std::string>();
			if (!vm.count("dependencies"))
				package_name = vm["output"].as<std::string>();
		}
		catch (const boost::program_options::error&)
		{
			show_usage();
			return 1;
		}
		catch (const boost::bad_any_cast&)
		{
			show_usage();
			return 1;
		}
	}

	std::vector<std::pair<String, std::map<std::string, std::string>>> entries;
	try
	{
		const auto document = IonDocument::open(Reader(index_name.c_str()));
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
		std::cerr << "ERROR(" << index_name << "): " << e.what() << "\n";
		return 1;
	}

	if (package_name.empty())
	{
		for (const auto& entry : entries)
			std::cout << entry.first << "\n";
		return 0;
	}

	const auto package = PackageWriter::create(package_name.c_str(), PackageType::Ypq);
	if (!package)
	{
		std::cerr << "ERROR(" << package_name << "): Can't open package file\n";
		return 1;
	}

	try
	{
		for (auto& entry : entries)
			package->add(entry.first, std::move(entry.second));
		if (!package->commit())
		{
			std::cerr << "ERROR(" << package_name << "): Unable to write package file\n";
			return 1;
		}
	}
	catch (const DataError& e)
	{
		std::cerr << "ERROR(" << index_name << "): " << e.what() << "\n";
		return 1;
	}
}
