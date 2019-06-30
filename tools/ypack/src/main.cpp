//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace
{
	template <typename... Args>
	void check(bool condition, Args&&... args)
	{
		if (!condition)
			throw Yttrium::DataError{ std::forward<Args>(args)... };
	}
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

	std::vector<std::string> paths;
	try
	{
		auto source = Yttrium::Source::from(index_name);
		check(static_cast<bool>(source), "Bad index file");
		Yttrium::IonReader ion{ *source };
		ion.read().check_name("package");
		ion.read().check_list_begin();
		for (auto token = ion.read(); token.type() != Yttrium::IonToken::Type::ListEnd; token = ion.read())
			paths.emplace_back(std::string{ token.to_value() });
		ion.read().check_end();
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << "ERROR(" << index_name << "): " << e.what() << "\n";
		return 1;
	}

	if (package_name.empty())
	{
		for (const auto& path : paths)
			std::cout << path << "\n";
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
		for (const auto& path : paths)
			package->add(path);
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
