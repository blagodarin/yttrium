# FAQ

## Overall

### Why no custom allocators?

Yttrium used to have custom allocators almost everywhere. However, they were
removed in favor of standard memory allocation facilities. Custom allocators
are not very useful if you don't intend to replace the standard memory manager
on a global scale (and I don't), don't need to catch memory leaks by yourself
(there is valgrind) and have no need of tracking memory usage per subsystem
(that's a cool feature, but it doesn't justify the means). If memory allocation
performance does matter, it's better to use more local facilities (e.g. memory
pools) then to pollute the entire codebase with custom allocation stuff.

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
