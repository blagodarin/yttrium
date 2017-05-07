#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>

#include <cstring>
#include <iostream>
#include <vector>

template <typename... Args>
void check(bool condition, Args&&... args)
{
	if (!condition)
		throw Yttrium::DataError{std::forward<Args>(args)...};
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

	std::vector<std::pair<std::string, std::map<std::string, std::string>>> entries;
	try
	{
		auto source = Yttrium::Source::from(index_name);
		check(static_cast<bool>(source), "Bad index file");
		Yttrium::IonReader ion{*source};
		ion.read().check_name("package");
		ion.read().check_list_begin();
		for (auto token = ion.read(); token.type() != Yttrium::IonReader::Token::Type::ListEnd;)
		{
			entries.emplace_back(Yttrium::strings::from_view(token.to_value()), std::map<std::string, std::string>{});
			token = ion.read();
			if (token.type() == Yttrium::IonReader::Token::Type::ObjectBegin) // TODO-17: Use init-statement.
			{
				for (token = ion.read(); token.type() != Yttrium::IonReader::Token::Type::ObjectEnd; token = ion.read())
				{
					auto property_name = Yttrium::strings::from_view(token.to_name());
					check(entries.back().second.count(property_name) == 0, "Duplicate property '", property_name, "'");
					entries.back().second.emplace(std::move(property_name), Yttrium::strings::from_view(ion.read().to_value()));
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
		for (const auto& entry : entries)
			std::cout << entry.first << "\n";
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
		for (auto& entry : entries)
			package->add(entry.first, std::move(entry.second));
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
