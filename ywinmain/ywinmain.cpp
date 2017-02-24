#include <vector>

#include <windows.h>

int main(int, char**);

// cppcheck-suppress unusedFunction
int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int argc = 0;
	const auto argv_w = ::CommandLineToArgvW(::GetCommandLineW(), &argc); // TODO: Use Y_UNIQUE_PTR.
	std::vector<char*> argv;
	for (int i = 0; i < argc; ++i)
	{
		const auto buffer_size = ::WideCharToMultiByte(CP_ACP, 0, argv_w[i], -1, nullptr, 0, nullptr, nullptr);
		argv.emplace_back(new char[buffer_size]);
		::WideCharToMultiByte(CP_ACP, 0, argv_w[i], -1, argv[i], buffer_size, nullptr, nullptr);
	}
	argv.emplace_back(nullptr);
	::LocalFree(argv_w);
	const auto result = main(argc, argv.data());
	for (const auto arg : argv)
		delete[] arg;
	return result;
}
