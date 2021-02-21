// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/main.h>

#include <primal/pointer.hpp>

#include <vector>

#include <windows.h>

namespace
{
	int call_ymain()
	{
		int argc = 0;
		std::vector<char*> argv;
		{
			const primal::CPtr<LPWSTR, ::LocalFree> argv_w{ ::CommandLineToArgvW(::GetCommandLineW(), &argc) };
			for (int i = 0; i < argc; ++i)
			{
				const auto buffer_size = ::WideCharToMultiByte(CP_UTF8, 0, argv_w[i], -1, nullptr, 0, nullptr, nullptr);
				argv.emplace_back(new char[buffer_size]);
				::WideCharToMultiByte(CP_UTF8, 0, argv_w[i], -1, argv[i], buffer_size, nullptr, nullptr);
			}
		}
		argv.emplace_back(nullptr);
		const auto result = ymain(argc, argv.data());
		for (const auto arg : argv)
			delete[] arg;
		return result;
	}
}

int main(int, char**)
{
	return call_ymain();
}

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return call_ymain();
}
