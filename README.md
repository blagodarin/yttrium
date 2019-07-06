[![Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)

| Branch | Travis CI | AppVeyor | Codecov |
|:------:|:---------:|:--------:|:-------:|
| [master](https://github.com/blagodarin/yttrium/tree/master) | [![Travis CI](https://travis-ci.org/blagodarin/yttrium.svg?branch=master)](https://travis-ci.org/blagodarin/yttrium/branches) | [![AppVeyor](https://ci.appveyor.com/api/projects/status/v3pco3lbvp2y4r9b/branch/master?svg=true)](https://ci.appveyor.com/project/blagodarin/yttrium/branch/master) | [![Codecov](https://codecov.io/gh/blagodarin/yttrium/branch/master/graph/badge.svg)](https://codecov.io/gh/blagodarin/yttrium/branch/master) |
| [develop](https://github.com/blagodarin/yttrium/tree/develop) | [![Travis CI](https://travis-ci.org/blagodarin/yttrium.svg?branch=develop)](https://travis-ci.org/blagodarin/yttrium/branches) | [![AppVeyor](https://ci.appveyor.com/api/projects/status/v3pco3lbvp2y4r9b/branch/develop?svg=true)](https://ci.appveyor.com/project/blagodarin/yttrium/branch/develop) | [![Codecov](https://codecov.io/gh/blagodarin/yttrium/branch/develop/graph/badge.svg)](https://codecov.io/gh/blagodarin/yttrium/branch/develop) |


# Yttrium

Yttrium is (intended to become) a game development toolkit.

Yttrium is an occasional hobby project, so don't expect it to be something
actually valuable or to eventually turn into one. It's more like a bunch of
various ideas piled together and roughly shaped to look game engine-ish.


## Build instructions

Building Yttrium requires [Git](https://git-scm.com/downloads/) to get it and
[CMake](https://cmake.org/download/) to build it. Yttrium also depends on some
third-party libraries; if you don't have them installed on your system, you can
get them using *Yttrium third-party bootstrapper (Y3)* which also requires Git
and CMake (and an Internet connection).

Check out CI configuration files ([.appveyor.yml](.appveyor.yml) and
[.travis.yml](.travis.yml)) for some hints on building Yttrium.

[Contribution guidelines](docs/contributing.md) may also help you find your
way around.
