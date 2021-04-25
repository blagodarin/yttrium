# Contribution guidelines

Thank you for your interest in contributing to Yttrium!

Yttrium aims to be:
* **Simple.** It should be clear how to use Yttrium just by looking at the
  example projects, and it should be clear how it works just by looking at its
  source code.
* **Up-to-date.** Yttrium should be kept compatible with the latest development
  tools and libraries available in the continuous integration systems used.
* **Concise.** Any supported platform, format, feature or technology is a
  liability that must be justified.
* **Comprehensive.** It should be possible to create a full-featured game for
  all supported platforms using only Yttrium libraries and tools (apart from the
  build system and the things provided by the Yttrium itself).

Please don't hesitate to make a contribution that helps Yttrium follow these
aims.


## Branches

`master` is a branch for Yttrium users. Each commit in this branch represents
a *usable* (not necessarily *stable*) state of the project. All CI builds
should succeed, and all tests should pass. All contributions are merged into
this branch.

To contribute, open a pull request, and when code review finishes and all
checks pass, it will be merged into `master`.

`develop` is a branch for Yttrium developers. All new changes go into this
branch first and are merged into `master` when they are ready, just like any
other contributions.


## Repository layout

Yttrium components are grouped into several top-level directories:
* `libs` contains libraries.
* `tools` contains command-line tools. Some of the tools are used to build
  libraries in `libs`.

Every component is placed in its own directory
(`libs/xxx` for `Y_xxx` library, `tools/xxx` for `yxxx` tool).

All components have the following structure:
* `src` contains source code of the component.
* `include` contains public headers which other components can use.
* `data` contains data required by the component.
* `tests` contains unit tests (also with `src` and possibly `data`).
* Files required to build the component (e. g. *CMakeLists.txt*) are placed
  into the component's root directory.
