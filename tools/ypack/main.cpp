#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

template <typename... Args>
void check(bool condition, Args&&... args)
{
	if (!condition)
		throw Yttrium::DataError{ std::forward<Args>(args)... };
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr
			<< "Usage:\n"
			<< "  ypack INDEX PACKAGE\n"
			<< "  ypack (--dependencies|-d) INDEX\n";
		return 1;
	}

	std::string index_name;
	std::string package_name;
	if (!std::strcmp(argv[1], "-d") || !std::strcmp(argv[1], "--dependencies"))
		index_name = argv[2];
	else
	{
		index_name = argv[1];
		package_name = argv[2];
	}

	std::vector<std::pair<std::string, std::map<std::string, std::string, std::less<>>>> entries;
	try
	{
		auto source = Yttrium::Source::from(index_name);
		check(static_cast<bool>(source), "Bad index file");
		Yttrium::IonReader ion{ *source };
		ion.read().check_name("package");
		ion.read().check_list_begin();
		for (auto token = ion.read(); token.type() != Yttrium::IonReader::Token::Type::ListEnd;)
		{
			auto& properties = entries.emplace_back(std::string{ token.to_value() }, std::map<std::string, std::string, std::less<>>{}).second;
			if (token = ion.read(); token.type() == Yttrium::IonReader::Token::Type::ObjectBegin)
			{
				for (token = ion.read(); token.type() != Yttrium::IonReader::Token::Type::ObjectEnd; token = ion.read())
				{
					const auto property_name = token.to_name();
					check(properties.count(property_name) == 0, "Duplicate property '", property_name, "'");
					properties.emplace(std::string{ property_name }, ion.read().to_value());
				}
				token = ion.read();
			}
		}
		ion.read().check_end();
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << "ERROR(" << index_name << "): " << e.what() << "\n";
		return 1;
	}

	if (package_name.empty())
	{
		for (const auto& [path, properties] : entries)
		{
			std::cout << path << "\n";
			std::ignore = properties;
		}
		return 0;
	}

	const auto package = Yttrium::PackageWriter::create(package_name, Yttrium::PackageType::Ypq);
	if (!package)
	{
		std::cerr << "ERROR(" << package_name << "): Can't open package file\n";
		return 1;
	}

	try
	{
		for (auto& [path, properties] : entries)
			package->add(path, std::move(properties));
		if (!package->commit())
		{
			std::cerr << "ERROR(" << package_name << "): Unable to write package file\n";
			return 1;
		}
	}
	catch (const Yttrium::DataError& e)
	{
		std::cerr << "ERROR(" << index_name << "): " << e.what() << "\n";
		return 1;
	}
}
