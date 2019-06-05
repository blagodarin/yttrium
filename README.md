[![Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)

| Branch | Travis CI | AppVeyor | Codecov |
|:------:|:---------:|:--------:|:-------:|
| master | [![Travis CI](https://travis-ci.org/blagodarin/yttrium.svg?branch=master)](https://travis-ci.org/blagodarin/yttrium) | [![AppVeyor](https://ci.appveyor.com/api/projects/status/v3pco3lbvp2y4r9b/branch/master?svg=true)](https://ci.appveyor.com/project/blagodarin/yttrium/branch/master) | [![Codecov](https://codecov.io/gh/blagodarin/yttrium/branch/master/graph/badge.svg)](https://codecov.io/gh/blagodarin/yttrium/branch/master) |
| develop | [![Travis CI](https://travis-ci.org/blagodarin/yttrium.svg?branch=develop)](https://travis-ci.org/blagodarin/yttrium) | [![AppVeyor](https://ci.appveyor.com/api/projects/status/v3pco3lbvp2y4r9b/branch/develop?svg=true)](https://ci.appveyor.com/project/blagodarin/yttrium/branch/develop) | [![Codecov](https://codecov.io/gh/blagodarin/yttrium/branch/develop/graph/badge.svg)](https://codecov.io/gh/blagodarin/yttrium/branch/develop) |


# Yttrium

Yttrium is (intended to become) a game development framework.

Yttrium is an occasional hobby project, so don't expect it to be something
actually valuable or to eventually turn into one. It's more like a bunch of
various ideas piled together and roughly shaped to look game engine-ish.

Yttrium aims to:
* Be a *modern C++ framework* in terms of API design, software architecture,
  development tools and coding practices.
* Provide flexible yet easy-to-use components for building interactive software.


## Build instructions

Building Yttrium requires [Git](https://git-scm.com/downloads/) to get it and
[CMake](https://cmake.org/download/) to build it. Yttrium also depends on some
third-party libraries; if you don't have them installed on your system, you can
get them using *Yttrium third-party bootstrapper (Y3)* which also requires Git
and CMake (and an Internet connection).

Check out CI configuration files ([.appveyor.yml](.appveyor.yml) and
[.travis.yml](.travis.yml)) for some hints on building Yttrium.


### Branches

`master` is a branch for Yttrium users. Each commit in this branch represents
a *usable* (not necessarily *stable*) state of the project. All CI builds
should succeed, and all tests should pass.

`develop` is a branch for Yttrium developers. All new changes go into this
branch first, and are merged into `master` when they are ready.
