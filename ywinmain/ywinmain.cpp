#include <windows.h>

int main(int, char**);

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int argc = 0;
	const auto argv_w = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
	const auto argv = new char*[argc + 1];
	for (int i = 0; i < argc; ++i)
	{
		const auto buffer_size = ::WideCharToMultiByte(CP_ACP, 0, argv_w[i], -1, nullptr, 0, nullptr, nullptr);
		argv[i] = new char[buffer_size];
		::WideCharToMultiByte(CP_ACP, 0, argv_w[i], -1, argv[i], buffer_size, nullptr, nullptr);
	}
	argv[argc] = nullptr;
	::LocalFree(argv_w);
	const auto result = main(argc, argv);
	for (int i = 0; i < argc; ++i)
		delete[] argv[i];
	delete[] argv;
	return result;
}
