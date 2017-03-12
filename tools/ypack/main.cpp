#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
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

	std::vector<std::pair<std::string, std::map<std::string, std::string>>> entries;
	try
	{
		const Reader reader{index_name.c_str()};
		check(static_cast<bool>(reader), "Bad index file");
		IonReader ion{reader};
		ion.read().check_name("package");
		ion.read().check_list_begin();
		for (auto token = ion.read(); token.type() != IonReader::Token::Type::ListEnd;)
		{
			entries.emplace_back(token.to_value().to_std(), std::map<std::string, std::string>());
			token = ion.read();
			if (token.type() == IonReader::Token::Type::ObjectBegin) // TODO-17: Use init-statement.
			{
				for (token = ion.read(); token.type() != IonReader::Token::Type::ObjectEnd; token = ion.read())
				{
					auto property_name = token.to_name().to_std();
					check(entries.back().second.count(property_name) == 0, "Duplicate property '", property_name, "'");
					entries.back().second.emplace(std::move(property_name), ion.read().to_value().to_std());
				}
				token = ion.read();
			}
		}
		ion.read().check_end();
	}
	catch (const IonError& e)
	{
		std::cerr << "ERROR(" << index_name << "): " << e.what() << "\n";
		return 1;
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
