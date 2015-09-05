# Yttrium

Originated in an attempt to create a game engine, Yttrium has currently become
an attic of code and a programming playground due to the lack of free time
to maintain and develop it properly.

Check the LICENSE, then use it if you like.

# FAQ

## Why not Google Test?

Google Test generates a lot of output for every test, all of which goes to one
output stream (stdout). Yttrium contains dozens of tests, which leads to huge
amounts of output for all the tests. It is pointless, it is hard to find which
tests failed and why, and it is impossible to turn the useless output off.

It does compile a bit faster than Boost Test, however, but the difference is
not very noticeable, and fast builds aren't the primary property of unit tests.
