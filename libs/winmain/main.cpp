#include "../core/utils/memory.h"

#include <vector>

#include <windows.h>

int main(int, char**);

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int argc = 0;
	std::vector<char*> argv;
	{
		const Yttrium::UniquePtr<LPWSTR, ::LocalFree> argv_w{::CommandLineToArgvW(::GetCommandLineW(), &argc)};
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
