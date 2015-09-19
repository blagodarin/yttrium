# FAQ

## Overall

### Why not Google Test?

Google Test generates some mandatory output for every test case and every test,
and all that output goes to one output stream (stdout). Yttrium contains dozens
of tests, testing it produces an unreadable amount of output, making it hard to
find which tests have failed and why.

It does compile a bit faster than Boost Test, however, but the difference is
not very noticeable, and fast builds aren't the primary property of unit tests.
