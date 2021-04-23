# TODO

## Y_storage

* Add zstd compression support.

* Consider using memory mapping for file sources.
  A source will then essentially become a memory range
  backed by some resource (file, allocation, another source),
  which can simplify source usage.
