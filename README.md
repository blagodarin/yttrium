[![Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)

| Branch | Travis CI | AppVeyor | Codecov |
|:------:|:---------:|:--------:|:-------:|
| master | [![Travis CI](https://travis-ci.org/blagodarin/yttrium.svg?branch=master)](https://travis-ci.org/blagodarin/yttrium) | [![AppVeyor](https://ci.appveyor.com/api/projects/status/v3pco3lbvp2y4r9b/branch/master?svg=true)](https://ci.appveyor.com/project/blagodarin/yttrium/branch/master) | [![Codecov](https://codecov.io/gh/blagodarin/yttrium/branch/master/graph/badge.svg)](https://codecov.io/gh/blagodarin/yttrium/branch/master) |
| develop | [![Travis CI](https://travis-ci.org/blagodarin/yttrium.svg?branch=develop)](https://travis-ci.org/blagodarin/yttrium) | [![AppVeyor](https://ci.appveyor.com/api/projects/status/v3pco3lbvp2y4r9b/branch/develop?svg=true)](https://ci.appveyor.com/project/blagodarin/yttrium/branch/develop) | [![Codecov](https://codecov.io/gh/blagodarin/yttrium/branch/develop/graph/badge.svg)](https://codecov.io/gh/blagodarin/yttrium/branch/develop) |


# Yttrium

Yttrium is (intended to become) a game development toolkit.

Yttrium is an occasional hobby project, so don't expect it to be something
actually valuable or to eventually turn into one. It's more like a bunch of
various ideas piled together and roughly shaped to look game engine-ish.


## Development

Yttrium aims to be:
* **Simple** to use and maintain.
* **Concise**: any supported platform, format, feature and technology is a
  liability that must be justified.
* **Modern** in terms of API design, software architecture, development tools
  used and coding practices followed.


### Branches

`master` is a branch for Yttrium users. Each commit in this branch represents
a *usable* (not necessarily *stable*) state of the project. All CI builds
should succeed, and all tests should pass.

`develop` is a branch for Yttrium developers. All new changes go into this
branch first, and are merged into `master` when they are ready.


### Build instructions

Building Yttrium requires [Git](https://git-scm.com/downloads/) to get it and
[CMake](https://cmake.org/download/) to build it. Yttrium also depends on some
third-party libraries; if you don't have them installed on your system, you can
get them using *Yttrium third-party bootstrapper (Y3)* which also requires Git
and CMake (and an Internet connection).

Check out CI configuration files ([.appveyor.yml](.appveyor.yml) and
[.travis.yml](.travis.yml)) for some hints on building Yttrium.


### Repository layout

Yttrium components are grouped into several top-level directories:
* `libs` contains public libraries.
* `tools` contains command-line tools. Some of the tools are used to build
  libraries in `libs`.
* `tests` contains unit tests for the public libraries, as well as other
  test-related stuff.
* `benchmarks` contains benchmarks for public library code.
* `examples` contains example applications build using Yttrium.

All components have the following structure:
* `src` contains source code of the component.
* `include` contains public headers which other components can use.
* `data` contains data required by the component.
* Files required to build the component (e. g. *CMakeLists.txt*) are placed
  into the component's root directory.
