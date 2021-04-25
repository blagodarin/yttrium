# Build instructions

*This document is under construction!*

Building Yttrium requires:
* [Git](https://git-scm.com/downloads/) as a version control system and as a
  tool used in the build process of some components.
* [CMake](https://cmake.org/download/) 3.19 or later.
* A supported compiler: MSVC 19.2x (Visual Studio 2019) for Windows, GCC 10 or
  Clang 11 for Linux. Yttrium uses some of the latest C++ language and library
  features, so it may be impossible to use older compilers.

You can also check out CI configuration files
([.appveyor.yml](/.appveyor.yml) and [.travis.yml](/.travis.yml))
for some hints on building Yttrium.
