# FAQ

## Overall

### Why not Google Test?

Google Test generates some mandatory output for every test case and every test,
and all that output goes to one output stream (stdout). Yttrium contains dozens
of tests, testing it produces an unreadable amount of output, making it hard to
find which tests have failed and why.

It does compile a bit faster than Boost Test, however, but the difference is
not very noticeable, and fast builds aren't the primary property of unit tests.

## Code style

### Why not #pragma once?

`#pragma once` is supported by GCC, and it is more compact and error-prone than
include guards. However, include guards make it possible to substitute actual
headers with mocks for unit testing. The feature isn't used here (yet), but is
planned to appear once, and `#pragma once` won't allow to do that. Also, it is
not that hard to design a simple and consistent include guard naming scheme.
