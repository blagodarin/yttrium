// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/main.h>

#include <primal/buffer.hpp>

#include <windows.h>

namespace
{
	int call_ymain()
	{
		primal::Buffer<char> buffer;
		primal::Buffer<char*> argv;
		int argc = 0;
		{
			const primal::CPtr<LPWSTR, ::LocalFree> argvW{ ::CommandLineToArgvW(::GetCommandLineW(), &argc) };
			primal::Buffer<int> sizes{ static_cast<size_t>(argc) };
			size_t bufferSize = 0;
			for (int i = 0; i < argc; ++i)
			{
				const auto size = ::WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, nullptr, 0, nullptr, nullptr);
				sizes.data()[i] = size;
				bufferSize += size;
			}
			buffer.reserve(bufferSize);
			argv.reserve(argc);
			size_t bufferOffset = 0;
			for (int i = 0; i < argc; ++i)
			{
				const auto data = buffer.data() + bufferOffset;
				const auto size = sizes.data()[i];
				::WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, data, size, nullptr, nullptr);
				argv.data()[i] = data;
				bufferOffset += size;
			}
		}
		return ymain(argc, argv.data());
	}
}

int main(int, char**)
{
	return ::call_ymain();
}

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return ::call_ymain();
}
