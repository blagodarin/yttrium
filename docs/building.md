# Build instructions

*This document is under construction!*

Building Yttrium requires:
* [Git](https://git-scm.com/downloads/) as a version control system and as a
  tool used in the build process of some components.
* [CMake](https://cmake.org/download/) 3.12 or later.
* A supported compiler: MSVC 19.22 (Visual Studio 2019) for Windows, GCC 9 or
  Clang 8 for Linux. Yttrium uses some of the latest C++ language and library
  features, so it may be impossible to use older compilers.

Yttrium depends on several third-party libraries and tools. Instead of requiring
them to be present in the build environment, Yttrium relies on a custom
CMake-based system called Y3 (for *Yttrium* and *third-party*) that is used to
download and build the required dependencies.

Right now you can check out CI configuration files
([.appveyor.yml](/.appveyor.yml) and [.travis.yml](/.travis.yml)) for some hints
on building Yttrium.
