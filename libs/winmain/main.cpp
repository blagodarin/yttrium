//
// Copyright 2018 Sergei Blagodarin
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

#include "../core/utils/memory.h"

#include <vector>

#include <windows.h>

int main(int, char**);

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int argc = 0;
	std::vector<char*> argv;
	{
		const Yttrium::UniquePtr<LPWSTR, ::LocalFree> argv_w{ ::CommandLineToArgvW(::GetCommandLineW(), &argc) };
		for (int i = 0; i < argc; ++i)
		{
			const auto buffer_size = ::WideCharToMultiByte(CP_ACP, 0, argv_w.get()[i], -1, nullptr, 0, nullptr, nullptr);
			argv.emplace_back(new char[buffer_size]);
			::WideCharToMultiByte(CP_ACP, 0, argv_w.get()[i], -1, argv[i], buffer_size, nullptr, nullptr);
		}
	}
	argv.emplace_back(nullptr);
	const auto result = main(argc, argv.data());
	for (const auto arg : argv)
		delete[] arg;
	return result;
}
